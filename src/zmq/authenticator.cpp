/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/protocol/zmq/authenticator.hpp>

#include <mutex>
#include <bitcoin/system.hpp>
#include <bitcoin/protocol/config/sodium.hpp>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/zmq/message.hpp>
#include <bitcoin/protocol/zmq/context.hpp>
#include <bitcoin/protocol/zmq/poller.hpp>
#include <bitcoin/protocol/zmq/socket.hpp>
#include <bitcoin/protocol/zmq/worker.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

using namespace bc::system;

// ZAP endpoint, see: rfc.zeromq.org/spec:27/ZAP
const config::endpoint authenticator::authentication_point("inproc://zeromq.zap.01");

// There may be only one authenticator per process.
authenticator::authenticator(thread_priority priority) NOEXCEPT
  : worker(priority),
    context_(false),
    require_allow_(false)
{
}

authenticator::~authenticator() NOEXCEPT
{
    stop();
}

authenticator::operator context&() NOEXCEPT
{
    return context_;
}

// Restartable after stop and not started on construct.
bool authenticator::start() NOEXCEPT
{
    // Context is thread safe, this critical section is for start atomicity.
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    std::unique_lock lock(stop_mutex_);
    BC_POP_WARNING()

    return context_.start() && worker::start();
    ///////////////////////////////////////////////////////////////////////////
}

bool authenticator::stop() NOEXCEPT
{
    // Context is thread safe, this critical section is for stop atomicity.
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    std::unique_lock lock(stop_mutex_);
    BC_POP_WARNING()

    // Stop the context first in case a blocking proxy is in use.
    return context_.stop() && worker::stop();
    ///////////////////////////////////////////////////////////////////////////
}

// The replier will never drop messages.
// rfc.zeromq.org/spec:27/ZAP/
void authenticator::work() NOEXCEPT
{
    socket replier(context_, zmq::socket::role::replier);

    if (!started(replier.bind(authentication_point) == error::success))
        return;

    poller poller;
    poller.add(replier);

    while (!poller.terminated() && !stopped())
    {
        if (!poller.wait().contains(replier.id()))
            continue;

        std::string version;
        std::string sequence;
        std::string status_code;
        std::string status_text;
        std::string userid;
        std::string metadata;

        message request;
        const auto ec = replier.receive(request);

        if (ec != error::success || request.size() < 6)
        {
            status_code = "500";
            status_text = "Internal error.";
        }
        else
        {
            version = request.dequeue_text();
            sequence = request.dequeue_text();
            const auto domain = request.dequeue_text();
            const auto address = request.dequeue_text();
            const auto identity = request.dequeue_text();
            const auto mechanism = request.dequeue_text();

            if (version != "1.0" || sequence.empty() || !identity.empty())
            {
                status_code = "500";
                status_text = "Internal error.";
            }
            else if (!allowed_address(address))
            {
                // Address restrictions are independent of mechanisms, but NULL
                // security requires a nonempty domain for this to be called.
                status_code = "400";
                status_text = "Address not enabled for access.";
            }
            else
            {
                if (mechanism == "NULL")
                {
                    // NULL ZAP calls are only made for non-empty domain.
                    // For PLAIN/CURVE, ZAP calls always made if running.
                    if (domain.empty())
                    {
                        status_code = "400";
                        status_text = "NULL mechanism requires domain.";
                    }
                    else if (request.size() != 0)
                    {
                        status_code = "400";
                        status_text = "Incorrect NULL parameterization.";
                    }
                    else if (!allowed_weak(domain))
                    {
                        status_code = "400";
                        status_text = "NULL mechanism not authorized.";
                    }
                    else
                    {
                        // It is more efficient to use an unsecured context or
                        // to not start the authenticator, but this works too.
                        status_code = "200";
                        status_text = "OK";
                        userid = "anonymous";
                    }
                }
                else if (mechanism == "CURVE")
                {
                    if (request.size() != 1)
                    {
                        status_code = "400";
                        status_text = "Incorrect CURVE parameterization.";
                    }
                    else
                    {
                        hash_digest public_key;

                        if (!request.dequeue(public_key))
                        {
                            status_code = "400";
                            status_text = "Invalid public key.";
                        }
                        else if (!allowed_key(public_key))
                        {
                            status_code = "400";
                            status_text = "Public key not authorized.";
                        }
                        else
                        {
                            status_code = "200";
                            status_text = "OK";
                            userid = "unspecified";
                        }
                    }
                }
                else if (mechanism == "PLAIN")
                {
                    if (request.size() != 2)
                    {
                        status_code = "400";
                        status_text = "Incorrect PLAIN parameterization.";
                    }
                    else
                    {
                        ////userid = request.dequeue_text();
                        ////const auto password = request.dequeue_text();
                        status_code = "400";
                        status_text = "PLAIN mechanism not supported.";
                    }
                }
                else
                {
                    status_code = "400";
                    status_text = "Security mechanism not supported.";
                }
            }
        }

        message response;
        response.enqueue(version);
        response.enqueue(sequence);
        response.enqueue(status_code);
        response.enqueue(status_text);
        response.enqueue(userid);
        response.enqueue(metadata);

        // This is returned to the zeromq ZAP dispatcher, not the caller.
        BC_DEBUG_ONLY(const code ec_ =) replier.send(response);
        BC_ASSERT(ec_ == error::success || ec_ == error::context_terminated);
    }

    finished(replier.stop());
}

// This must be called on the socket thread.
// Addresses and client keys may be updated after this is applied.
// The configuration at the time of this call determines the mode of security.
bool authenticator::apply(socket& socket, const std::string& domain,
    bool secure) NOEXCEPT
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    property_mutex_.lock_shared();
    const auto& private_key = private_key_;
    const auto have_public_keys = !keys_.empty();
    const auto require_domain = !secure && !adresses_.empty();
    property_mutex_.unlock_shared();
    ///////////////////////////////////////////////////////////////////////////

    // A private server key is required if there are public client keys.
    if ((have_public_keys && !private_key) ||
        (require_domain && domain.empty()))
        return false;

    // Weak domain list persists after socket close so don't reuse domains.
    if (require_domain)
    {
        BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
        weak_domains_.emplace(domain);
        BC_POP_WARNING()
        return socket.set_authentication_domain(domain);
    }

    // All three values are required for secure configuration.
    return !secure || (private_key &&
        socket.set_private_key(private_key) &&
        socket.set_curve_server() &&
        socket.set_authentication_domain(domain));
}

void authenticator::set_private_key(const sodium& private_key) NOEXCEPT
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    std::unique_lock lock(property_mutex_);
    BC_POP_WARNING()

    private_key_ = private_key;
    ///////////////////////////////////////////////////////////////////////////
}

bool authenticator::allowed_address(const std::string& ip_address) const NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    std::shared_lock lock(property_mutex_);

    const auto entry = adresses_.find(ip_address);
    const auto found = entry != adresses_.end();
    return (require_allow_ && found && entry->second) ||
        (!require_allow_ && (!found || entry->second));
    ///////////////////////////////////////////////////////////////////////////
    BC_POP_WARNING()
}

bool authenticator::allowed_key(const hash_digest& public_key) const NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    std::shared_lock lock(property_mutex_);

    return keys_.empty() || keys_.find(public_key) != keys_.end();
    ///////////////////////////////////////////////////////////////////////////
    BC_POP_WARNING()
}

bool authenticator::allowed_weak(const std::string& domain) const NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
        std::shared_lock lock(property_mutex_);

    return weak_domains_.find(domain) != weak_domains_.end();
    ///////////////////////////////////////////////////////////////////////////
    BC_POP_WARNING()
}

void authenticator::allow(const hash_digest& public_key) NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    std::unique_lock lock(property_mutex_);

    keys_.emplace(public_key);
    ///////////////////////////////////////////////////////////////////////////
    BC_POP_WARNING()
}

void authenticator::allow(const config::authority& address) NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    std::unique_lock lock(property_mutex_);

    require_allow_ = true;

    // Due to emplace behavior, first writer wins allow/deny conflict.
    adresses_.emplace(address.to_host(), true);
    ///////////////////////////////////////////////////////////////////////////
    BC_POP_WARNING()
}

void authenticator::deny(const config::authority& address) NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    std::unique_lock lock(property_mutex_);

    // Denial is effective independent of whitelisting.
    // Due to emplace behavior, first writer wins allow/deny conflict.
    adresses_.emplace(address.to_host(), false);
    ///////////////////////////////////////////////////////////////////////////
    BC_POP_WARNING()
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

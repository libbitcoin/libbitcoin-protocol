/*
 * Copyright (c) 2011-2016 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin-protocol.
 *
 * libbitcoin-protocol is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) 
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/protocol/zmq/authenticator.hpp>

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <zmq.h>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/protocol/zmq/message.hpp>
#include <bitcoin/protocol/zmq/socket.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

#define NAME "authenticator"

// ZAP endpoint, see: rfc.zeromq.org/spec:27/ZAP
static const auto zap_endpoint = "inproc://zeromq.zap.01";

static constexpr uint32_t polling_interval_milliseconds = 1;

authenticator::authenticator(threadpool& pool)
  : socket_(*this, socket::role::router),
    dispatch_(pool, NAME),
    require_address_(false)
{
}

bool authenticator::start()
{
    // There may be only one such endpoint per process, tied to one context.
    if (self() == nullptr || !socket_.bind(zap_endpoint))
        return false;

    // The authenticator establishes a well-known endpoint.
    poller_.add(socket_);

    // The dispatched thread closes when the monitor loop exits (stop).
    // This requires that the threadpool be joined prior to this destruct.
    dispatch_.concurrent(
        std::bind(&authenticator::monitor, this));

    return true;
}

// github.com/zeromq/rfc/blob/master/src/spec_27.c
void authenticator::monitor()
{
    // Ignore expired and keep looping, exiting the thread when terminated.
    while (!poller_.terminated())
    {
        const auto socket_id = poller_.wait(polling_interval_milliseconds);

        // If the id doesn't match the poll is either terminated or expired.
        if (socket_id != socket_.id())
            continue;

        data_chunk origin;
        data_chunk delimiter;
        std::string version;
        std::string sequence;
        std::string status_code;
        std::string status_text;
        std::string userid;
        std::string metadata;

        message request;
        const auto received = request.receive(socket_);

        if (!received || request.size() < 8)
        {
            status_code = "500";
            status_text = "Internal error.";
        }
        else
        {
            origin = request.dequeue_data();
            delimiter = request.dequeue_data();
            version = request.dequeue_text();
            sequence = request.dequeue_text();
            const auto domain = request.dequeue_text();
            const auto address = request.dequeue_text();
            const auto identity = request.dequeue_text();
            const auto mechanism = request.dequeue_text();

            // Each socket on the authenticated context must set a domain.
            if (origin.empty() || !delimiter.empty() || version != "1.0" ||
                sequence.empty() || domain.empty() || !identity.empty())
            {
                status_code = "500";
                status_text = "Internal error.";
            }
            else if (!allowed(address))
            {
                // Address restrictions are independent of mechanisms.
                status_code = "400";
                status_text = "Address not enabled for access.";
            }
            else
            {
                if (mechanism == "NULL")
                {
                    if (request.size() != 0)
                    {
                        status_code = "400";
                        status_text = "Incorrect NULL parameterization.";
                    }
                    else if (!keys_.empty())
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
                        else if (!allowed(public_key))
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
        response.enqueue(origin);
        response.enqueue(delimiter);
        response.enqueue(version);
        response.enqueue(sequence);
        response.enqueue(status_code);
        response.enqueue(status_text);
        response.enqueue(userid);
        response.enqueue(metadata);

        DEBUG_ONLY(const auto sent =) response.send(socket_);
        BITCOIN_ASSERT_MSG(sent, "Failed to send ZAP response.");
    }

    const auto stopped = socket_.stop();
    BITCOIN_ASSERT_MSG(stopped, "Failed to close socket.");

    if (stopped)
        poller_.clear();
}

bool authenticator::allowed(const hash_digest& public_key) const
{
    return !keys_.empty() || keys_.find(public_key) != keys_.end();
}

bool authenticator::allowed(const std::string& ip_address) const
{
    return !require_address_ || adresses_.find(ip_address) != adresses_.end();
}

void authenticator::allow(const hash_digest& public_key)
{
    keys_.emplace(public_key, true);
}

void authenticator::allow(const config::authority& address)
{
    require_address_ = true;
    adresses_.emplace(address.to_hostname(), true);
}

void authenticator::deny(const config::authority& address)
{
    // Denial is effective independent of whitelisting.
    adresses_.emplace(address.to_hostname(), false);
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

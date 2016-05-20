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

// ZAP endpoint, see: rfc.zeromq.org/spec:27/ZAP
static const auto zap_enpoint = "inproc://zeromq.zap.01";
static constexpr uint32_t polling_interval_milliseconds = 1;

authenticator::authenticator(context& context)
  : socket_(context, socket::role::router),
    require_key_(false),
    require_address_(false),
    stopped_(true),
    interval_milliseconds_(polling_interval_milliseconds)
{
    if (socket_.bind(zap_enpoint))
    {
        poller_.add(socket_);
        stopped_.store(false);
        thread_ = std::make_shared<std::thread>(
            std::bind(&authenticator::monitor, this));
    }
}

authenticator::~authenticator()
{
    if (!stopped_.load())
    {
        stopped_.store(true);
        thread_->join();
    }
}

authenticator::operator const bool() const
{
    return !stopped_.load();
}

// Handle ZAP authentication requests.
// github.com/zeromq/rfc/blob/master/src/spec_27.c
void authenticator::monitor()
{
    while (!stopped_)
    {
        // The id can be zero (none) or socket_ id.
        if (poller_.wait(interval_milliseconds_) == socket_.id())
        {
            // An address delimiter frame, which SHALL have a length of zero.
            // The version frame, which SHALL contain the three octets "1.0".
            // The request id, which MAY contain an opaque binary blob.
            // The domain, which SHALL contain a string.
            // The address, the origin network IP address.
            // The identity, the connection Identity, if any.
            // The mechanism, which SHALL contain a string.
            // The credentials, which SHALL be zero or more opaque frames.
            message request;
            const auto received = request.receive(socket_);
            BITCOIN_ASSERT_MSG(received, "Internal malformed ZAP request.");

            if (!received || request.size() < 8)
                continue;

            const auto origin = request.dequeue_data();
            const auto delimiter = request.dequeue_data();

            if (!delimiter.empty())
                continue;

            const auto version = request.dequeue_text();

            if (version != "1.0")
                continue;

            // TODO: use domain to associate socket (ZMQ_ZAP_DOMAIN)??
            const auto sequence = request.dequeue_text();
            /* const auto domain = */ request.dequeue_text();
            const auto address = request.dequeue_text();

            if (!allowed(address))
                continue;

            // TODO: use domain to associate socket (ZMQ_ZAP_DOMAIN)??
            /* const auto identity = */ request.dequeue_text();
            const auto mechanism = request.dequeue_text();

            std::string status_code;
            std::string status_text;
            std::string userid;
            std::string metadata;

            if (mechanism == "NULL")
            {
                if (!request.empty())
                {
                    status_code = "400";
                    status_text = "Incorrect parameter count for null.";
                }
                else
                {
                    status_code = "200";
                    status_text = "OK";
                    userid = "anonymous";
                }
            }
            else if (mechanism == "PLAIN")
            {
                if (request.size() != 2)
                {
                    status_code = "400";
                    status_text = "Incorrect parameter count for plain.";
                }
                else
                {
                    const auto username = request.dequeue_text();
                    const auto password = request.dequeue_text();

                    status_code = "400";
                    status_text = "Plain security mechanism not supported.";
                }
            }
            else if (mechanism == "CURVE")
            {
                if (request.size() != 1)
                {
                    status_code = "400";
                    status_text = "Incorrect parameter count for curve.";
                }
                else
                {
                    hash_digest public_key;
                    if (!request.dequeue(public_key) || !allowed(public_key))
                    {
                        status_code = "400";
                        status_text = "Invalid client public key.";
                    }
                    else
                    {
                        status_code = "200";
                        status_text = "OK";
                        userid = "unspecified";
                    }
                }
            }
            else
            {
                status_code = "400";
                status_text = "Security mechanism not supported.";
            }

            // An address delimiter frame, which SHALL have a length of zero.
            // The version frame, which SHALL contain the three octets "1.0".
            // The request id, which MAY contain an opaque binary blob.
            // The status code, which SHALL contain a string.
            // The status text, which MAY contain a string.
            // The user id, which SHALL contain a string.
            // The metadata, which MAY contain a blob.
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
    }
}

bool authenticator::allowed(const hash_digest& public_key)
{
    return !require_key_ || keys_.find(public_key) != keys_.end();
}

bool authenticator::allowed(const std::string& ip_address)
{
    return !require_address_ || adresses_.find(ip_address) != adresses_.end();
}

void authenticator::allow(const hash_digest& public_key)
{
    require_key_ = true;
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

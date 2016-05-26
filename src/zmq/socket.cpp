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
#include <bitcoin/protocol/zmq/socket.hpp>

#include <cstdint>
#include <string>
#include <zmq.h>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/protocol/zmq/certificate.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

static constexpr int32_t zmq_true = 1;
static constexpr int32_t zmq_fail = -1;
static constexpr int32_t zmq_send_buffer = 1000;
static constexpr int32_t zmq_receive_buffer = 1000;
static constexpr int32_t zmq_linger_milliseconds = 10;

socket::socket()
  : socket(nullptr)
{
}

// zmq_term terminates blocking operations but blocks until either each socket
// in the context has been explicitly closed or the linger period is exceeded.
socket::socket(void* zmq_socket)
  : socket_(zmq_socket),
    send_buffer_(zmq_send_buffer),
    receive_buffer_(zmq_receive_buffer)
{
}

socket::socket(context& context, role socket_role)
  : socket(zmq_socket(context.self(), to_socket_type(socket_role)))
{
    if (socket_ == nullptr)
        return;

    if (!set(ZMQ_SNDHWM, send_buffer_) || !set(ZMQ_RCVHWM, receive_buffer_) ||
        !set(ZMQ_LINGER, zmq_linger_milliseconds))
    {
        stop();
    }
}

socket::~socket()
{
    stop();
}

int32_t socket::to_socket_type(role socket_role)
{
    switch (socket_role)
    {
        case role::pair: return ZMQ_PAIR;
        case role::publisher: return ZMQ_PUB;
        case role::subscriber: return ZMQ_SUB;
        case role::requester: return ZMQ_REQ;
        case role::replier: return ZMQ_REP;
        case role::dealer: return ZMQ_DEALER;
        case role::router: return ZMQ_ROUTER;
        case role::puller: return ZMQ_PULL;
        case role::pusher: return ZMQ_PUSH;
        case role::extended_publisher: return ZMQ_XPUB;
        case role::extended_subscriber: return ZMQ_XSUB;
        case role::streamer: return ZMQ_STREAM;
        default: return -1;
    }
}

bool socket::stop()
{
    if (socket_ == nullptr)
        return false;

    const auto closed = zmq_close(socket_) != zmq_fail;
    socket_ = nullptr;
    return closed;
}

socket::operator const bool() const
{
    return socket_ != nullptr;
}

void socket::assign(socket&& other)
{
    // Free any existing socket resources.
    stop();

    // Assume ownership of the other socket's state.
    socket_ = other.socket_;

    // Don't destroy other socket's resource as it would destroy ours.
    other.socket_ = nullptr;
}

bool socket::bind(const std::string& address)
{
    return zmq_bind(socket_, address.c_str()) != zmq_fail;
}

bool socket::connect(const std::string& address)
{
    return zmq_connect(socket_, address.c_str()) != zmq_fail;
}

bool socket::set(int32_t option, int32_t value)
{
    return zmq_setsockopt(socket_, option, &value, sizeof(value)) != zmq_fail;
}

bool socket::set(int32_t option, const std::string& value)
{
    if (value.empty())
        return true;

    const auto buffer = value.c_str();
    return zmq_setsockopt(socket_, option, buffer, value.size()) != zmq_fail;
}

bool socket::set_authentication_domain(const std::string& domain)
{
    return set(ZMQ_ZAP_DOMAIN, domain);
}

bool socket::set_curve_server()
{
    return set(ZMQ_CURVE_SERVER, zmq_true);
}

bool socket::set_curve_client(const std::string& server_public_key)
{
    return set(ZMQ_CURVE_SERVERKEY, server_public_key);
}

bool socket::set_public_key(const std::string& key)
{
    return set(ZMQ_CURVE_PUBLICKEY, key);
}

bool socket::set_private_key(const std::string& key)
{
    return set(ZMQ_CURVE_SECRETKEY, key);
}

bool socket::set_certificate(const certificate& certificate)
{
    return certificate && set_public_key(certificate.public_key()) &&
        set_private_key(certificate.private_key());
}

void* socket::self()
{
    return socket_;
}

socket::identifier socket::id() const
{
    return reinterpret_cast<socket::identifier>(socket_);
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

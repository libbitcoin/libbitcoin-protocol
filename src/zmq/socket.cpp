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
#include <bitcoin/protocol/zmq/socket.hpp>

#include <algorithm>
#include <bitcoin/system.hpp>
#include <bitcoin/protocol/config/sodium.hpp>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/settings.hpp>
#include <bitcoin/protocol/zmq/authenticator.hpp>
#include <bitcoin/protocol/zmq/certificate.hpp>
#include <bitcoin/protocol/zmq/error.hpp>
#include <bitcoin/protocol/zmq/identifiers.hpp>
#include <bitcoin/protocol/zmq/message.hpp>
#include <bitcoin/protocol/zmq/zeromq.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

using namespace bc::system;
static const bc::protocol::settings default_settings;

// Linger
// The default value of -1 specifies an infinite linger period. Pending
// messages shall not be discarded after a call to zmq_close(); attempting to
// terminate the socket's context with zmq_term() shall block until all pending
// messages have been sent to a peer. The value 0 specifies no linger period.

// High Water
// api.zeromq.org/4-2:zmq-socket -and- zeromq.org/area:faq#toc6
// On PUB and ROUTER sockets, when the SNDHWM is reached on outgoing pipes,
// messages are dropped. DEALER and PUSH sockets will block when their
// outgoing pipes are full. On incoming pipes, SUB sockets will drop received
// messages when they reach their RCVHWM. PULL and DEALER sockets will refuse
// new messages and force messages to wait upstream due to TCP backpressure.

constexpr int32_t seconds(uint32_t value) NOEXCEPT
{
    // TODO: use std::chrono.
    constexpr uint32_t ms_to_seconds = 1000;
    constexpr auto milliseconds = sign_cast<uint32_t>(max_int32);
    constexpr auto limit = milliseconds / ms_to_seconds;
    return possible_narrow_sign_cast<int32_t>(
        std::min(value, limit) * ms_to_seconds);
}

int32_t socket::to_socket_type(role socket_role) NOEXCEPT
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

// Because self is only set on construct, sockets are not restartable.
// zmq_term terminates blocking operations but blocks until each socket in the
// context is explicitly closed. Socket close kills transfers after linger.
socket::socket(void* zmq_socket) NOEXCEPT
  : self_(zmq_socket),
    identifier_(reinterpret_cast<identifier>(zmq_socket))
{
}

socket::socket(context& context, role socket_role) NOEXCEPT
  : socket(context, socket_role, default_settings)
{
}

// This design presumes no setting value should be able to cause failure.
socket::socket(context& context, role socket_role,
    const settings& settings) NOEXCEPT
  : socket(zmq_socket(context.self(), to_socket_type(socket_role)))
{
    if (self_ == nullptr)
        return;

    // TODO: update authenticator to normalize IP addresses for matching.
    // Currently this is simplified to string matching and so limited to ipv4.
    if (/*!set(ZMQ_IPV6, zmq_true) ||*/
        !set32(ZMQ_LINGER, zmq_false) ||
        !set32(ZMQ_SNDHWM, limit<int32_t>(settings.send_high_water)) ||
        !set32(ZMQ_RCVHWM, limit<int32_t>(settings.receive_high_water)) ||
        !set32(ZMQ_HANDSHAKE_IVL, seconds(settings.handshake_seconds)) ||
        !set32(ZMQ_HEARTBEAT_IVL, seconds(settings.ping_seconds)))
    {
        stop();
        return;
    }

    const auto size_limit = limit<int64_t>(settings.message_size_limit);

    if (!set64(ZMQ_MAXMSGSIZE, size_limit == 0 ? -1 : size_limit))
    {
        stop();
        return;
    }

    const auto inactivity = seconds(settings.inactivity_seconds);

    // There is also ZMQ_HEARTBEAT_TTL (not used currently).
    if (!set32(ZMQ_HEARTBEAT_TIMEOUT, inactivity))
    {
        stop();
        return;
    }

    const auto send = settings.send_milliseconds;

    // Zero sets infinite wait (default), no way to set zero/immediate.
    if (!set32(ZMQ_SNDTIMEO, send == 0 ? -1 : send))
    {
        stop();
        return;
    }

    const auto reconnect = seconds(settings.reconnect_seconds);

    // Zero disables, reconnect_interval is hardwired to the default (100ms).
    if (!set32(ZMQ_RECONNECT_IVL, reconnect == 0 ? -1 : zmq_reconnect_interval) ||
        !set32(ZMQ_RECONNECT_IVL_MAX, reconnect))
    {
        stop();
        return;
    }

    // Limited to subscriber sockets (not configured, always set by default).
    if (socket_role == role::subscriber && !set(ZMQ_SUBSCRIBE, zmq_subscribe_all))
    {
        stop();
        return;
    }
}

socket::~socket() NOEXCEPT
{
    stop();
}

bool socket::stop() NOEXCEPT
{
    if (self_ == nullptr)
        return true;

    if (zmq_close(self_) == zmq_fail)
        return false;

    self_ = nullptr;
    return true;
}

socket::operator bool() const NOEXCEPT
{
    return self_ != nullptr;
}

void* socket::self() NOEXCEPT
{
    return self_;
}

// To preserve identity the id survives after the socket is destroyed.
identifier socket::id() const NOEXCEPT
{
    return identifier_;
}

error::code socket::bind(const config::endpoint& address) NOEXCEPT
{
    if (zmq_bind(self_, address.to_uri().c_str()) == zmq_fail)
        return error::get_last_error();

    return error::success;
}

error::code socket::connect(const config::endpoint& address) NOEXCEPT
{
    if (zmq_connect(self_, address.to_uri().c_str()) == zmq_fail)
        return error::get_last_error();

    return error::success;
}

// private
bool socket::set32(int32_t option, int32_t value) NOEXCEPT
{
    return zmq_setsockopt(self_, option, &value, sizeof(value)) != zmq_fail;
}

// private
bool socket::set64(int32_t option, int64_t value) NOEXCEPT
{
    return zmq_setsockopt(self_, option, &value, sizeof(value)) != zmq_fail;
}

// private
bool socket::set(int32_t option, const std::string& value) NOEXCEPT
{
    const auto buffer = value.c_str();
    return zmq_setsockopt(self_, option, buffer, value.size()) != zmq_fail;
}

// private
bool socket::set(int32_t option, const data_chunk& value) NOEXCEPT
{
    return zmq_setsockopt(self_, option, value.data(), value.size())
        != zmq_fail;
}

// For NULL security, ZAP calls are only made for non-empty domain.
// For PLAIN/CURVE, calls are always made if ZAP handler is present.
bool socket::set_authentication_domain(const std::string& domain) NOEXCEPT
{
    return domain.empty() || set(ZMQ_ZAP_DOMAIN, domain);
}

// Defines whether the socket will act as server for CURVE security.
bool socket::set_curve_server() NOEXCEPT
{
    return set32(ZMQ_CURVE_SERVER, zmq_true);
}

// Sets socket's long term server key, must set this on CURVE client sockets.
bool socket::set_curve_client(const sodium& server_public_key) NOEXCEPT
{
    return server_public_key &&
        set(ZMQ_CURVE_SERVERKEY, server_public_key.to_string());
}

// Sets socket's long term public key, must set this on CURVE client sockets.
bool socket::set_public_key(const sodium& key) NOEXCEPT
{
    return key && set(ZMQ_CURVE_PUBLICKEY, key.to_string());
}

// You must set this on both CURVE client and server sockets.
bool socket::set_private_key(const sodium& key) NOEXCEPT
{
    return key && set(ZMQ_CURVE_SECRETKEY, key.to_string());
}

// Use on client for both set_public_key and set_private_key from a cert.
// If CURVE is not required by server, call set_certificate({ null_hash })
// to generate an arbitrary client certificate for a secure socket.
bool socket::set_certificate(const certificate& certificate) NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return certificate &&
        set_public_key(certificate.public_key().to_string()) &&
        set_private_key(certificate.private_key().to_string());
    BC_POP_WARNING()
}

bool socket::set_socks_proxy(const config::authority& socks_proxy) NOEXCEPT
{
    return is_nonzero(socks_proxy.port()) &&
        set(ZMQ_SOCKS_PROXY, socks_proxy.to_string());
}

bool socket::set_subscription(const data_chunk& filter) NOEXCEPT
{
    return set(ZMQ_SUBSCRIBE, filter);
}

bool socket::set_unsubscription(const data_chunk& filter) NOEXCEPT
{
    return set(ZMQ_UNSUBSCRIBE, filter);
}

error::code socket::send(message& packet) NOEXCEPT
{
    return packet.send(*this);
}

error::code socket::receive(message& packet) NOEXCEPT
{
    return packet.receive(*this);
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

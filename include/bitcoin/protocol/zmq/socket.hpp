/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_PROTOCOL_ZMQ_SOCKET_HPP
#define LIBBITCOIN_PROTOCOL_ZMQ_SOCKET_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <bitcoin/protocol/config/authority.hpp>
#include <bitcoin/protocol/config/endpoint.hpp>
#include <bitcoin/protocol/config/sodium.hpp>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/settings.hpp>
#include <bitcoin/protocol/zmq/certificate.hpp>
#include <bitcoin/protocol/zmq/context.hpp>
#include <bitcoin/protocol/zmq/error.hpp>
#include <bitcoin/protocol/zmq/identifiers.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

class message;
class authenticator;

/// This class is not thread safe.
/// All calls must be made on the socket thread.
/// Because the socket is only set on construct, sockets are not restartable.
class BCP_API socket
  : public enable_shared_from_base<socket>, noncopyable
{
public:
    /// The full set of socket roles defined by zeromq.
    enum class role
    {
        pair,
        publisher,
        subscriber,
        requester,
        replier,
        dealer,
        router,
        puller,
        pusher,
        extended_publisher,
        extended_subscriber,
        streamer
    };

    /// A shared socket pointer.
    typedef std::shared_ptr<socket> ptr;

    /// Construct a socket from an existing zeromq socket.
    socket(void* zmq_socket);

    /// Construct a socket of the given context and role and default settings.
    /// Subscribers are automatically subscribed to all messages.
    socket(context& context, role socket_role);

    /// Construct a socket of the given context and role.
    /// Subscribers are automatically subscribed to all messages.
    socket(context& context, role socket_role, const settings& settings);

    /// Close the socket.
    virtual ~socket();

    /// Close the socket (optional, must close or destroy before context stop).
    virtual bool stop();

    /// True if the socket is valid.
    operator bool() const;

    /// The underlying zeromq socket.
    void* self();

    /// An opaque locally unique idenfier, valid after stop.
    identifier id() const;

    /// Bind the socket to the specified local address.
    error::code bind(const endpoint& address);

    /// Connect the socket to the specified remote address.
    error::code connect(const endpoint& address);

    /// Sets the domain for ZAP (ZMQ RFC 27) authentication.
    bool set_authentication_domain(const std::string& domain);

    /// Configure the socket as a curve server (also set the secrety key).
    bool set_curve_server();

    /// Configure the socket as client to the curve server.
    bool set_curve_client(const sodium& server_public_key);

    /// Apply the specified public key to the socket.
    bool set_public_key(const sodium& key);

    /// Apply the specified private key to the socket.
    bool set_private_key(const sodium& key);

    /// Apply the keys of the specified certificate to the socket.
    bool set_certificate(const certificate& certificate);

    /// Configure the socket to connect through the specified socks5 proxy.
    bool set_socks_proxy(const authority& socks_proxy);

    /// Configure subscriber socket to apply the message filter.
    bool set_subscription(const system::data_chunk& filter);

    /// Configure subscriber socket to remove the message filter.
    bool set_unsubscription(const system::data_chunk& filter);

    /// Send a message on this socket.
    error::code send(message& packet);

    /// Receive a message from this socket.
    error::code receive(message& packet);

protected:
    static int to_socket_type(role socket_role);

    bool set32(int32_t option, int32_t value);
    bool set64(int32_t option, int64_t value);
    bool set(int32_t option, const std::string& value);
    bool set(int32_t option, const system::data_chunk& value);

private:
    void* self_;
    const identifier identifier_;
};

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif

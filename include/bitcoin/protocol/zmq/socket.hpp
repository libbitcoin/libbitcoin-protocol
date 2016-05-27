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
#ifndef LIBBITCOIN_PROTOCOL_ZMQ_SOCKET_HPP
#define LIBBITCOIN_PROTOCOL_ZMQ_SOCKET_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/zmq/certificate.hpp>
#include <bitcoin/protocol/zmq/context.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

/// This class is not thread safe.
class BCP_API socket
  : public enable_shared_from_base<socket>
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

    /// A locally unique idenfitier for this socket.
    typedef intptr_t identifier;

    socket();
    socket(void* zmq_socket);
    socket(context& context, role socket_role);

    /// This class is not const copyable.
    socket(const socket&) = delete;
    void operator=(const socket&) = delete;

    /// Free socket resources.
    virtual ~socket();

    /// True if the socket is valid.
    operator const bool() const;

    /// The underlying zeromq socket.
    void* self();

    /// The socket identifier is an opaue correlation idenfier.
    identifier id() const;

    /// Transfer ownership of this socket to another.
    void assign(socket&& other);

    /// Bind the socket to the specified local address.
    bool bind(const config::endpoint& address);

    /// Connect the socket to the specified remote address.
    bool connect(const config::endpoint& address);

    /// Sets the domain for ZAP (ZMQ RFC 27) authentication.
    bool set_authentication_domain(const std::string& domain);

    /// Configure the socket as a curve server (also set the secrety key).
    bool set_curve_server();

    /// Configure the socket as client to the curve server.
    bool set_curve_client(const config::sodium& server_public_key);

    /// Apply the specified public key to the socket.
    bool set_public_key(const config::sodium& key);

    /// Apply the specified private key to the socket.
    bool set_private_key(const config::sodium& key);

    /// Apply the keys of the specified certificate to the socket.
    bool set_certificate(const certificate& certificate);

    /// Close the socket.
    bool stop();

private:
    static int to_socket_type(role socket_role);
    bool set(int32_t option, int32_t value);
    bool set(int32_t option, const std::string& value);

    void* socket_;
    const int32_t send_buffer_;
    const int32_t receive_buffer_;
};

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif

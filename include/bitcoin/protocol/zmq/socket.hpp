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
#include <bitcoin/protocol/zmq/context.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

class BCP_API socket
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

    // A locally unique idenfitier for this socket.
    typedef intptr_t identifier;

    socket();
    socket(void* zmq_socket);
    socket(context& context, role socket_role);

    /// This class is not const copyable.
    socket(const socket&) = delete;
    void operator=(const socket&) = delete;

    /// Free socket resources.
    ~socket();

    /// True if there is an encapsultaed zeromq socket.
    operator const bool() const;

    /// True if the encapsultaed zeromq sockets are the same.
    bool operator==(const socket& other) const;

    /// True if the encapsultaed zeromq sockets are not the same.
    bool operator!=(const socket& other) const;

    /// The underlying zeromq socket.
    void* self();
    void* self() const;

    /// The port to whicih the socket is bound, or zero.
    uint16_t port() const;

    /// The socket identifier is an opaue correlation idenfier.
    identifier id() const;

    /// Transfer ownership of this socket to another.
    void assign(socket&& other);

    /// Bind the socket to the specified local address.
    bool bind(const std::string& address);

    /// Connect the socket to the specified remote address.
    bool connect(const std::string& address);

    bool set_curve_server();
    bool set_curve_serverkey(const std::string& key);
    bool set_zap_domain(const std::string& domain);

private:
    static int to_socket_type(role socket_role);

    bool set(int32_t option, int32_t value);
    bool set(int32_t option, const std::string& value);
    void initialize(context& context, role socket_role);
    bool destroy();

    // The encapsulated zeromq socket.
    void* socket_;
    uint16_t port_;
    const int32_t send_buffer_;
    const int32_t receive_buffer_;
    const int32_t linger_milliseconds_;
};

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif

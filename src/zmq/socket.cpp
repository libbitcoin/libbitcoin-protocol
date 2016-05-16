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

#include <czmq.h>
#include <bitcoin/bitcoin.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

socket::socket()
  : self_(nullptr)
{
}

socket::socket(void* self)
  : self_(self)
{
}

socket::socket(socket&& other)
{
    BITCOIN_ASSERT(self_ == nullptr);

    self_ = other.self_;
    other.self_ = nullptr;
}

socket::~socket()
{
    if (!self_)
        return;

    // TODO: configure linger milliseconds, default -1 (infinite).

    ////auto rc = zmq_setsockopt(zocket, ZMQ_LINGER, &linger, sizeof(int));
    ////assert(rc == 0 || zmq_errno() == ETERM);
    zsocket_set_linger(self_, 0);

    ////zmq_close (zocket);
    ////zsocket_destroy(context.self(), self_);
}

// TODO: map zmq types to integer class enum.
socket::socket(context& context, int type)
{
    // TODO: configure these int settings as uint16_t.
    // TODO: configure sndhwm, default 0, 1000 in zctx_new.
    // TODO: configure rcvhwm, default 0, 1000 in zctx_new.

    ////self_ = zmq_socket(context.self(), type);
    ////auto rc = zmq_setsockopt(self_, ZMQ_SNDHWM, &sndhwm, sizeof(int));
    ////assert(rc == 0 || zmq_errno() == ETERM);
    ////auto rc = zmq_setsockopt(self_, ZMQ_RCVHWM, &rcvhwm, sizeof(int));
    ////assert(rc == 0 || zmq_errno() == ETERM);
    self_ = zsocket_new(context.self(), type);
}

// TODO: convert to bool.
int socket::bind(const std::string& address)
{
    ////int port = zmq_bind (self, endpoint);
    return zsocket_bind(self_, address.c_str());
}

// TODO: convert to bool.
int socket::connect(const std::string& address)
{
    ////Returns 0 if the endpoint is valid, -1 if the connect failed.
    ////zmq_connect (self, endpoint);
    return zsocket_connect(self_, address.c_str());
}

void socket::set_curve_server()
{
    ////int rc = zmq_setsockopt(zocket, ZMQ_CURVE_SERVER, &curve_server, sizeof(int));
    ////assert(rc == 0 || zmq_errno() == ETERM);
    zsocket_set_curve_server(self_, 1);
}

void socket::set_curve_serverkey(const std::string& key)
{
    ////int rc = zmq_setsockopt(zocket, ZMQ_CURVE_SERVERKEY, curve_serverkey, strlen(curve_serverkey));
    ////assert(rc == 0 || zmq_errno() == ETERM);
    zsocket_set_curve_serverkey(self_, key.c_str());
}

void socket::set_zap_domain(const std::string& domain)
{
    ////int rc = zmq_setsockopt(zocket, ZMQ_ZAP_DOMAIN, zap_domain, strlen(zap_domain));
    ////assert(rc == 0 || zmq_errno() == ETERM);
    zsocket_set_zap_domain(self_, domain.c_str());
}

void* socket::self()
{
    return self_;
}

void* socket::self() const
{
    return self_;
}

socket::operator const bool() const
{
    return self_ != nullptr;
}

bool socket::operator==(const socket& other) const
{
    return self_ == other.self_;
}

bool socket::operator!=(const socket& other) const
{
    return !(*this == other);
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

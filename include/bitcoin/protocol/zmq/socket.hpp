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

#include <string>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/zmq/context.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

class BCP_API socket
{
public:
    socket();
    socket(void* self);
    socket(socket&& other);
    socket(context& context, int type);

    /// This class is not copyable.
    socket(const socket&) = delete;
    void operator=(const socket&) = delete;

    ~socket();

    void* self();
    void* self() const;

    operator const bool() const;
    bool operator==(const socket& other) const;
    bool operator!=(const socket& other) const;

    int bind(const std::string& address);
    int connect(const std::string& address);

    void set_curve_server();
    void set_curve_serverkey(const std::string& key);
    void set_zap_domain(const std::string& domain);

private:
    void* self_;
};

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif

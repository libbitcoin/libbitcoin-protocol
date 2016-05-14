/*
 * Copyright (c) 2011-2013 libczmq++ developers (see AUTHORS)
 *
 * This file is part of libczmq++.
 *
 * libczmq++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
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
#ifndef LIBBITCOIN_PROTOCOL_ZMQ_MESSAGE_HPP
#define LIBBITCOIN_PROTOCOL_ZMQ_MESSAGE_HPP

#include <cstdint>
#include <vector>
#include <czmq.h>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/protocol/zmq/socket.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

class BCP_API message
{
public:
    void append(const bc::data_chunk& part);
    void append(bc::data_chunk&& part);
    const bc::data_stack& parts() const;
    bool send(socket& sock);
    bool receive(socket& sock);

private:
    bc::data_stack parts_;
};

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif


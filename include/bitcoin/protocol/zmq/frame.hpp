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
#ifndef LIBBITCOIN_PROTOCOL_ZMQ_FRAME_HPP
#define LIBBITCOIN_PROTOCOL_ZMQ_FRAME_HPP

#include <zmq.h>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/zmq/socket.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

class BCP_API frame
{
public:
    frame();
    frame(const data_chunk& data);
    ~frame();

    bool more() const;
    data_chunk payload();

    bool receive(socket& socket);
    bool send(socket& socket, bool more);
    bool destroy();

private:
    static bool initialize(zmq_msg_t& message, const data_chunk& data);
    bool set_more(socket& socket);

    bool more_;
    const bool valid_;
    zmq_msg_t message_;
};

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif


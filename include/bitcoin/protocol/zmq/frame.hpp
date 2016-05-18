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

#include <bitcoin/bitcoin.hpp>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/zmq/socket.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

class BCP_API frame
{
public:
    /// Construct a frame with no payload (for receiving).
    frame();

    /// Construct a frame with the specified payload (for sending).
    frame(const data_chunk& data);

    /// Free the frame's allocated memory.
    ~frame();

    /// This class is not copyable.
    frame(const frame&) = delete;
    void operator=(const frame&) = delete;

    /// True if the construction was successful.
    operator const bool() const;

    /// True if there is more data to receive.
    bool more() const;

    /// The initialized or received payload of the frame.
    data_chunk payload();

    /// Receive a frame on the socket.
    bool receive(socket& socket);

    /// Send a frame on the socket.
    bool send(socket& socket, bool more);

private:
    // zmq_msg_t alias, keeps zmq.h out of our headers.
    typedef union zmq_msg{ unsigned char alignment[64]; void* pointer; };

    static bool initialize(zmq_msg& message, const data_chunk& data);

    bool set_more(socket& socket);
    bool destroy();

    bool more_;
    const bool valid_;
    zmq_msg message_;
};

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif


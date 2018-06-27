/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_PROTOCOL_ZMQ_FRAME_HPP
#define LIBBITCOIN_PROTOCOL_ZMQ_FRAME_HPP

#include <memory>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/zmq/socket.hpp>
#include <bitcoin/protocol/zmq/zeromq.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

/// This class is not thread safe.
class BCP_API frame
  : public enable_shared_from_base<frame>, noncopyable
{
public:
    /// A shared frame pointer.
    typedef std::shared_ptr<frame> ptr;

    /// Construct a frame with no payload (for receiving).
    frame();

    /// Construct a frame with the specified payload (for sending).
    frame(const data_chunk& data);

    /// Free the frame's allocated memory.
    virtual ~frame();

    /// True if the construction was successful.
    operator bool() const;

    /// True if there is more data to receive.
    bool more() const;

    /// The initialized or received payload of the frame.
    data_chunk payload() const;

    /// Must be called on the socket thread.
    /// Receive a frame on the socket.
    code receive(socket& socket);

    /// Must be called on the socket thread.
    /// Send a frame on the socket.
    code send(socket& socket, bool more);

private:
    bool initialize(const data_chunk& data);
    bool set_more(socket& socket);
    bool destroy();

    bool more_;
    const bool valid_;

    // Mutability is a hack around lack of const overloads in zeromq, allowing
    // the payload method to be properly const while other access is not.
    mutable zmq_msg message_;
};

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif


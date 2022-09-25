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
#ifndef LIBBITCOIN_PROTOCOL_ZMQ_MESSAGE_HPP
#define LIBBITCOIN_PROTOCOL_ZMQ_MESSAGE_HPP

#include <cstddef>
#include <string>
#include <bitcoin/system.hpp>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/zmq/error.hpp>
#include <bitcoin/protocol/zmq/socket.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

/// This class is not thread safe.
class BCP_API message
{
public:
    /// A zeromq route identifier is always this size.
    static const size_t address_size = 5;

    /// An identifier for message routing.
    typedef system::data_array<address_size> address;

    /// Add an unsigned integer message part to the outgoing message.
    template <typename Unsigned>
    void enqueue_little_endian(Unsigned value) NOEXCEPT
    {
        queue_.emplace(system::to_chunk(
            system::to_little_endian<Unsigned>(value)));
    }

    /// Remove an unsigned from the queue top, false if empty queue or invalid.
    template <typename Unsigned>
    bool dequeue(Unsigned& value) NOEXCEPT
    {
        if (queue_.empty())
            return false;

        const auto& front = queue_.front();

        if (front.size() == sizeof(Unsigned))
        {
            value = system::from_little_endian<Unsigned>(front);
            queue_.pop();
            return true;
        }

        queue_.pop();
        return false;
    }

    /// Add an empty message part to the outgoing message.
    void enqueue() NOEXCEPT;

    /// Move a data message part to the outgoing message.
    void enqueue(system::data_chunk&& value) NOEXCEPT;

    /// Add a data message part to the outgoing message.
    void enqueue(const system::data_chunk& value) NOEXCEPT;

    /// Add a text message part to the outgoing message.
    void enqueue(const std::string& value) NOEXCEPT;

    /// Move an identifier message part to the outgoing message.
    void enqueue(const address& value) NOEXCEPT;

    /// Remove a message part from the top of the queue, empty if empty queue.
    system::data_chunk dequeue_data() NOEXCEPT;
    std::string dequeue_text() NOEXCEPT;

    /// Remove a part from the queue top, false if empty queue or invalid.
    bool dequeue() NOEXCEPT;
    bool dequeue(system::data_chunk& value) NOEXCEPT;
    bool dequeue(std::string& value) NOEXCEPT;
    bool dequeue(system::hash_digest& value) NOEXCEPT;
    bool dequeue(address& value) NOEXCEPT;

    /// Clear the queue of message parts.
    void clear() NOEXCEPT;

    /// True if the queue is empty.
    bool empty() const NOEXCEPT;

    /// The number of items on the queue.
    size_t size() const NOEXCEPT;

    /// Must be called on the socket thread.
    /// Send the message in parts. If a send fails the unsent parts remain.
    error::code send(socket& socket) NOEXCEPT;

    /// Must be called on the socket thread.
    /// Receve a message (clears the queue first).
    error::code receive(socket& socket) NOEXCEPT;

protected:
    data_queue queue_;
};

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif


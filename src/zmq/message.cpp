/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/protocol/zmq/message.hpp>

#include <algorithm>
#include <string>
#include <utility>
#include <bitcoin/system.hpp>
#include <bitcoin/protocol/zmq/error.hpp>
#include <bitcoin/protocol/zmq/frame.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

using namespace bc::system;

message::message() NOEXCEPT
  : queue_{}
{
}

void message::enqueue() NOEXCEPT
{
    queue_.emplace();
}

void message::enqueue(data_chunk&& value) NOEXCEPT
{
    queue_.push(std::move(value));
}

void message::enqueue(const data_chunk& value) NOEXCEPT
{
    queue_.push(value);
}

void message::enqueue(const std::string& value) NOEXCEPT
{
    queue_.push(to_chunk(value));
}

void message::enqueue(const address& value) NOEXCEPT
{
    queue_.push(to_chunk(value));
}

bool message::dequeue() NOEXCEPT
{
    if (queue_.empty())
        return false;

    queue_.pop();
    return true;
}

bool message::dequeue(data_chunk& value) NOEXCEPT
{
    if (queue_.empty())
        return false;

    value = dequeue_data();
    return true;
}

bool message::dequeue(std::string& value) NOEXCEPT
{
    if (queue_.empty())
        return false;

    value = dequeue_text();
    return true;
}

bool message::dequeue(address& value) NOEXCEPT
{
    if (queue_.empty())
        return false;

    const auto& front = queue_.front();

    if (front.size() == address_size)
    {
        std::copy(front.begin(), front.end(), value.begin());
        queue_.pop();
        return true;
    }

    queue_.pop();
    return false;
}

// Used by ZAP for public/private key read/write.
bool message::dequeue(hash_digest& value) NOEXCEPT
{
    if (queue_.empty())
        return false;

    const auto& front = queue_.front();

    if (front.size() == hash_size)
    {
        std::copy(front.begin(), front.end(), value.begin());
        queue_.pop();
        return true;
    }

    queue_.pop();
    return false;
}

data_chunk message::dequeue_data() NOEXCEPT
{
    if (queue_.empty())
        return {};

    const auto data = queue_.front();
    queue_.pop();
    return data;
}

std::string message::dequeue_text() NOEXCEPT
{
    if (queue_.empty())
        return {};

    auto text = to_string(queue_.front());
    queue_.pop();
    return text;
}

void message::clear() NOEXCEPT
{
    while (!queue_.empty())
        queue_.pop();
}

bool message::empty() const NOEXCEPT
{
    return queue_.empty();
}

size_t message::size() const NOEXCEPT
{
    return queue_.size();
}

// Must be called on the socket thread.
error::code message::send(socket& socket) NOEXCEPT
{
    while (!queue_.empty())
    {
        frame part{ queue_.front() };
        queue_.pop();
        const auto ec = part.send(socket, queue_.empty());

        if (ec)
            return ec;
    }

    return error::success;
}

// Must be called on the socket thread.
error::code message::receive(socket& socket) NOEXCEPT
{
    clear();
    auto done = false;

    while (!done)
    {
        frame frame{};
        const auto ec = frame.receive(socket);

        if (ec)
            return ec;

        queue_.push(frame.payload());
        done = !frame.more();
    }

    return error::success;
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

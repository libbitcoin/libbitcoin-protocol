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
#include <bitcoin/protocol/zmq/message.hpp>

#include <czmq.h>
#include <bitcoin/bitcoin.hpp>

static_assert (sizeof(byte) == sizeof(uint8_t), "Incorrect byte size");

namespace libbitcoin {
namespace protocol {
namespace zmq {

void message::append(const data_chunk& part)
{
    parts_.push_back(part);
}

void message::append(data_chunk&& part)
{
    parts_.emplace_back(std::move(part));
}

const data_stack& message::parts() const
{
    return parts_;
}

bool message::send(socket& sock)
{
    int flags = ZFRAME_MORE;
    const auto last = parts_.end();

    for (auto part = parts_.begin(); part != last; ++part)
    {
        if (part == last - 1)
            flags = 0;

        auto frame = zframe_new(part->data(), part->size());

        if (zframe_send(&frame, sock.self(), flags) == -1)
            return false;
    }

    return true;
}

bool message::receive(socket& sock)
{
    auto done = false;

    while (!done)
    {
        auto frame = zframe_recv(sock.self());

        if (frame == nullptr)
        {
            zframe_destroy(&frame);
            return false;
        }

        done = zframe_more(frame) == 0;
        auto first = zframe_data(frame);
        auto last = first + zframe_size(frame);
        parts_.push_back({ first, last });
        zframe_destroy(&frame);
    }

    return true;
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

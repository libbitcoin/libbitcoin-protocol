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
#include <bitcoin/protocol/zmq/message.hpp>

#include <bitcoin/bitcoin.hpp>
#include <bitcoin/protocol/zmq/frame.hpp>

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
    auto count = parts_.size();

    for (const auto& part: parts_)
    {
        frame frame(part);

        if (!frame.send(sock, --count == 0))
            return false;
    }

    return true;
}

bool message::receive(socket& sock)
{
    auto done = false;

    while (!done)
    {
        frame frame;

        if (!frame.receive(sock))
            return false;

        parts_.emplace_back(frame.payload());
        done = !frame.more();
    }

    return true;
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

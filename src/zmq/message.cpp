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

#include <string>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/protocol/zmq/frame.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

void message::append(data_chunk&& part)
{
    parts_.emplace_back(std::move(part));
}

void message::append(const std::string& part)
{
    append(data_chunk{ part.begin(), part.end() });
}

void message::append(const data_chunk& part)
{
    parts_.push_back(part);
}

const data_stack& message::parts() const
{
    return parts_;
}

std::string message::text() const
{
    if (parts_.empty())
        return{};

    const auto& first = parts_.front();
    return std::string(first.begin(), first.end());
}

void message::clear()
{
    parts_.clear();
}

bool message::send(socket& sock)
{
    auto count = parts_.size();

    for (auto it = parts_.begin(); it != parts_.end(); it = parts_.erase(it))
    {
        frame frame(*it);

        if (!frame.send(sock, --count == 0))
            return false;
    }

    BITCOIN_ASSERT(parts_.empty());
    return true;
}

bool message::receive(socket& sock)
{
    auto done = false;
    clear();

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

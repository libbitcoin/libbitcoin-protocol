/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/protocol/packet.hpp>

#include <bitcoin/bitcoin.hpp>
#include <bitcoin/protocol/zmq/message.hpp>
#include <bitcoin/protocol/zmq/socket.hpp>

namespace libbitcoin {
namespace protocol {

packet::packet()
{
}

const data_chunk packet::origin() const
{
    return origin_;
}

const data_chunk packet::destination() const
{
    return destination_;
}

void packet::set_destination(const data_chunk& destination)
{
    destination_ = destination;
}

bool packet::receive(zmq::socket& socket)
{
    zmq::message message;

    if (!message.receive(socket))
        return false;

    const auto& parts = message.parts();

    if (parts.size() == 0)
        return true;

    auto it = parts.begin();

    // Extract reply address(es)
    if (parts.size() > 2)
    {
        origin_ = *it;
        ++it;
    }

    // Confirm empty delimiter frame
    ++it;

    // Parse request payload from data frame
    if (decode_payload(*it))
        ++it;

    BITCOIN_ASSERT(it == parts.end());
    return true;
}

bool packet::receive(const std::shared_ptr<zmq::socket>& socket)
{
    if (socket == nullptr)
        return false;

    return receive(*(socket.get()));
}

bool packet::send(zmq::socket& socket)
{
    zmq::message message;

    if (!destination_.empty())
        message.append(destination_);

    message.append({});

    if (!encode_payload(message))
        return false;

    message.send(socket);
    return true;
}

bool packet::send(const std::shared_ptr<zmq::socket>& socket)
{
    if (socket == nullptr)
        return false;

    return send(*(socket.get()));
}

}
}

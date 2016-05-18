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
#ifndef LIBBITCOIN_PROTOCOL_ZMQ_MESSAGE_HPP
#define LIBBITCOIN_PROTOCOL_ZMQ_MESSAGE_HPP

#include <string>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/protocol/zmq/socket.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

class BCP_API message
{
public:
    /// Add a message part to the outgoing message.
    void append(data_chunk&& part);
    void append(const data_chunk& part);
    void append(const std::string& part);

    // Obtain the parts of the created or read message.
    const data_stack& parts() const;

    // Obtain the first part as a text string, or empty if no parts.
    std::string text() const;

    /// Clear the stack of message parts.
    void clear();

    /// Send the message in parts. If a send fails the unsent parts remain.
    bool send(socket& sock);

    /// Receve a message (clears the stack first).
    bool receive(socket& sock);

private:
    data_stack parts_;
};

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif


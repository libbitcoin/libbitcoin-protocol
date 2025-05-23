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
#ifndef LIBBITCOIN_PROTOCOL_ZMQ_IDENTIFIER_HPP
#define LIBBITCOIN_PROTOCOL_ZMQ_IDENTIFIER_HPP

#include <bitcoin/protocol/define.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

/// A locally unique idenfitier for this socket.
typedef intptr_t identifier;

/// This class is not thread safe.
/// An indicator for a set of socket idenfitiers.
class BCP_API identifiers
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(identifiers);

    /// Construct.
    identifiers() NOEXCEPT;

    /// True if the result set contains no identifiers.
    bool empty() const NOEXCEPT;

    /// True if the result set contains the identifier.
    bool contains(identifier value) const NOEXCEPT;

    // Allow poller to push identifiers.
    void push(const void* socket) NOEXCEPT;

private:
    std::vector<identifier> ids_;
};

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif

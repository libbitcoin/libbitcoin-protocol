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
#ifndef LIBBITCOIN_PROTOCOL_WEB_WEBSOCKET_TRANSFER_HPP
#define LIBBITCOIN_PROTOCOL_WEB_WEBSOCKET_TRANSFER_HPP

#include <cstddef>
#include <bitcoin/system.hpp>
#include <bitcoin/protocol/define.hpp>

namespace libbitcoin {
namespace protocol {
namespace http {

struct BCP_API websocket_transfer
{
    bool in_progress;
    size_t offset;
    size_t length;
    size_t header_length;
    system::data_chunk mask;
    system::data_chunk data;
};

} // namespace http
} // namespace protocol
} // namespace libbitcoin

#endif

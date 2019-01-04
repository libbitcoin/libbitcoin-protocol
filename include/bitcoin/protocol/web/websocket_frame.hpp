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
#ifndef LIBBITCOIN_PROTOCOL_WEB_WEBSOCKET_FRAME_HPP
#define LIBBITCOIN_PROTOCOL_WEB_WEBSOCKET_FRAME_HPP

#include <cstddef>
#include <cstdint>
#include <bitcoin/system.hpp>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/web/event.hpp>
#include <bitcoin/protocol/web/websocket_op.hpp>

namespace libbitcoin {
namespace protocol {
namespace http {

class BCP_API websocket_frame
{
public:
    static system::data_chunk to_header(size_t length, websocket_op code);

    websocket_frame(const uint8_t* data, size_t size);

    operator bool() const;
    bool final() const;
    bool fragment() const;
    event event_type() const;
    websocket_op op_code() const;
    uint8_t flags() const;
    size_t header_length() const;
    size_t data_length() const;
    size_t mask_length() const;

private:
    void from_data(const uint8_t* data, size_t read_length);

    static const size_t mask_ = 4;

    bool valid_;
    uint8_t flags_;
    size_t header_;
    size_t data_;
};

} // namespace http
} // namespace protocol
} // namespace libbitcoin

#endif

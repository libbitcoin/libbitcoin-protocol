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
#include <bitcoin/protocol/settings.hpp>

#include <cstdint>
#include <bitcoin/system.hpp>

namespace libbitcoin {
namespace protocol {

using namespace bc::system;

settings::settings()
  : send_high_water(100),
    receive_high_water(100),
    message_size_limit(0),
    handshake_seconds(30),
    ping_seconds(0),
    inactivity_seconds(0),
    reconnect_seconds(1),
    send_milliseconds(0)
{
}

settings::settings(uint32_t send_high_water, uint32_t receive_high_water)
  : send_high_water(send_high_water),
    receive_high_water(receive_high_water),
    message_size_limit(0), 
    handshake_seconds(30),
    ping_seconds(0),
    inactivity_seconds(0),
    reconnect_seconds(1),
    send_milliseconds(0)
{
}

} // namespace protocol
} // namespace libbitcoin

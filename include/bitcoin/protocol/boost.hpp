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
#ifndef LIBBITCOIN_PROTOCOL_BOOST_HPP
#define LIBBITCOIN_PROTOCOL_BOOST_HPP

#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>
#include <boost/thread.hpp>

namespace libbitcoin {
namespace protocol {
    
typedef boost::thread thread;
typedef boost::asio::ip::address_v4 ipv4;
typedef boost::asio::ip::address_v6 ipv6;

} // namespace protocol
} // namespace libbitcoin

#endif

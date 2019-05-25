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
#ifndef LIBBITCOIN_PROTOCOL_WEB_DEFAULT_PAGE_DATA_HPP
#define LIBBITCOIN_PROTOCOL_WEB_DEFAULT_PAGE_DATA_HPP

#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/settings.hpp>

namespace libbitcoin {
namespace protocol {
namespace http {

/// Given endpoints for each web service based on user configuration,
/// we can generate default page data.
std::string get_default_page_data(const bc::system::config::endpoint& query,
    const bc::system::config::endpoint& heartbeat,
    const bc::system::config::endpoint& block,
    const bc::system::config::endpoint& transaction);

} // namespace http
} // namespace protocol
} // namespace libbitcoin

#endif

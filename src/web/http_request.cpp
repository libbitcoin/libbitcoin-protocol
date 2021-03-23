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
#include <bitcoin/protocol/web/http_request.hpp>

#include <string>
#include <boost/algorithm/string.hpp>
#include <bitcoin/protocol/web/http.hpp>

namespace libbitcoin {
namespace protocol {
namespace http {

http_request::http_request()
  : method({}), uri({}), protocol({}), protocol_version(0.0f),
    message_length(0), content_length(0), headers({}), parameters({}),
    upgrade_request(false), json_rpc(false)
{
}

std::string http_request::find(const string_map& haystack,
    const std::string& needle) const
{
    const auto it = haystack.find(needle);
    return it == haystack.end() ? std::string{} : it->second;
}

std::string http_request::header(const std::string& header) const
{
    return find(headers, boost::algorithm::to_lower_copy(header));
}

std::string http_request::parameter(const std::string& parameter) const
{
    return find(parameters, boost::algorithm::to_lower_copy(parameter));
}

} // namespace http
} // namespace protocol
} // namespace libbitcoin

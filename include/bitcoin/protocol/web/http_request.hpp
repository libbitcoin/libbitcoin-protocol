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
#ifndef LIBBITCOIN_PROTOCOL_WEB_REQUEST_HPP
#define LIBBITCOIN_PROTOCOL_WEB_REQUEST_HPP

#include <cstddef>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/web/http.hpp>
#include <bitcoin/protocol/web/protocol_status.hpp>

namespace libbitcoin {
namespace protocol {
namespace http {

// TODO: move implementation to cpp.
class BCP_API http_request
{
public:
    http_request()
      : method({}),
        uri({}),
        protocol({}),
        protocol_version(0.0f),
        message_length(0),
        content_length(0),
        headers({}),
        parameters({}),
        upgrade_request(false),
        json_rpc(false)
    {
    }

    std::string find(const string_map& haystack,
        const std::string& needle) const
    {
        const auto it = haystack.find(needle);
        return it == haystack.end() ? std::string{} : it->second;
    }

    std::string header(std::string header) const
    {
        boost::algorithm::to_lower(header);
        return find(headers, header);
    }

    std::string parameter(std::string parameter) const
    {
        boost::algorithm::to_lower(parameter);
        return find(parameters, parameter);
    }

    std::string method;
    std::string uri;
    std::string protocol;
    double protocol_version;
    size_t message_length;
    size_t content_length;
    string_map headers;
    string_map parameters;
    bool upgrade_request;
    bool json_rpc;
    boost::property_tree::ptree json_tree;
};

} // namespace http
} // namespace protocol
} // namespace libbitcoin

#endif

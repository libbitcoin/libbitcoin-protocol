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
#ifndef LIBBITCOIN_PROTOCOL_WEB_JSON_STRING_HPP
#define LIBBITCOIN_PROTOCOL_WEB_JSON_STRING_HPP

#include <cstdint>
#include <functional>
#include <string>
#include <sstream>
#include <bitcoin/system.hpp>
#include <bitcoin/protocol/define.hpp>
#include <boost/property_tree/ptree.hpp>

namespace libbitcoin {
namespace protocol {
namespace http {

// Object to JSON converters.
//-----------------------------------------------------------------------------

BCP_API std::string to_json(const boost::property_tree::ptree& tree);
BCP_API std::string to_json(const boost::property_tree::ptree& tree,
    uint32_t id);
BCP_API std::string to_json(uint64_t height, uint32_t id);
BCP_API std::string to_json(const system::hash_digest& hash, uint32_t id);
BCP_API std::string to_json(const system::code& code, uint32_t id);
BCP_API std::string to_json(const system::chain::header& header, uint32_t id);
BCP_API std::string to_json(const system::chain::block& block, uint32_t id);
BCP_API std::string to_json(const system::chain::block& block, uint32_t height,
    uint32_t id);
BCP_API std::string to_json(const system::chain::transaction& transaction,
    uint32_t id);

// Object to JSON rpc converters.
//-----------------------------------------------------------------------------

// The rpc distinction is for formatting the json to conform to
// libbitcoin json, or rpc json.
namespace rpc {

BCP_API std::string to_json(const boost::property_tree::ptree& tree,
    uint32_t id);
BCP_API std::string to_json(uint64_t height, uint32_t id);
BCP_API std::string to_json(const system::hash_digest& hash, uint32_t id);
BCP_API std::string to_json(const system::code& code, uint32_t id);
BCP_API std::string to_json(const system::chain::header& header, uint32_t id);
BCP_API std::string to_json(const system::chain::block& block, uint32_t id);
BCP_API std::string to_json(const system::chain::block& block, uint32_t height,
    uint32_t id);
BCP_API std::string to_json(const system::chain::transaction& transaction,
    uint32_t id);

} // namespace rpc

} // namespace http
} // namespace protocol
} // namespace libbitcoin

#endif

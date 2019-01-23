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
#include <bitcoin/protocol/web/json_string.hpp>

// Explicitly use std::placeholders here for usage internally to the
// boost parsing helpers included from json_parser.hpp.
// See: https://svn.boost.org/trac10/ticket/12621
using namespace std::placeholders;

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

// TODO: include other headers.

namespace libbitcoin {
namespace protocol {
namespace http {

using namespace bc::system;
using namespace bc::system::chain;
using namespace bc::system::config;
using namespace boost::property_tree;

// Object to JSON converters.
//-----------------------------------------------------------------------------

std::string to_json(const ptree& tree)
{
    std::stringstream json_stream;
    write_json(json_stream, tree);
    return json_stream.str();
}

std::string to_json(const ptree& tree, uint32_t id)
{
    ptree result_tree;
    result_tree.add_child("result", tree);
    result_tree.put("id", id);
    return to_json(result_tree);
}

std::string to_json(uint64_t height, uint32_t id)
{
    return to_json(property_tree(height, id));
}

std::string to_json(const code& code, uint32_t id)
{
    return to_json(property_tree(code, id));
}

std::string to_json(const hash_digest& hash, uint32_t id)
{
    auto tree = property_tree(hash);
    tree.put("id", id);
    return to_json(tree);
}

std::string to_json(const chain::header& header, uint32_t id)
{
    return to_json(property_tree(config::header(header)), id);
}

std::string to_json(const chain::block& block, uint32_t id)
{
    return to_json(property_tree(block, true), id);
}

std::string to_json(const chain::block& block, uint32_t, uint32_t id)
{
    return to_json(property_tree(config::header(block.header())), id);
}

std::string to_json(const chain::transaction& transaction, uint32_t id)
{
    return to_json(property_tree(config::transaction(transaction), true), id);
}

// Object to JSON rpc converters.
//-----------------------------------------------------------------------------

namespace rpc {

std::string to_json(const ptree& tree, uint32_t id)
{
    ptree result_tree;
    result_tree.add_child("result", tree);
    result_tree.put("id", id);
    return http::to_json(result_tree);
}

std::string to_json(uint64_t height, uint32_t id)
{
    ptree tree;
    tree.put("result", height);
    tree.put("id", id);
    return http::to_json(tree);
}

std::string to_json(const code& code, uint32_t id)
{
    ptree tree;
    ptree error_tree;
    error_tree.put("code", code.value());
    error_tree.put("message", code.message());
    tree.add_child("error", error_tree);
    tree.put("id", id);
    return http::to_json(tree);
}

std::string to_json(const hash_digest& hash, uint32_t id)
{
    ptree tree;
    tree.put("result", encode_hash(hash));
    tree.put("id", id);
    return http::to_json(tree);
}

std::string to_json(const chain::header& header, uint32_t id)
{
    auto hex = [](uint32_t value)
    {
        std::stringstream hex_value;
        hex_value << std::setfill('0') << std::setw(sizeof(uint32_t) * 2);
        hex_value << std::hex << value;
        return hex_value.str();
    };

    ptree tree;
    tree.put("hash", encode_hash(header.hash()));
    tree.put("version", header.version());
    tree.put("versionHex", hex(header.version()));
    tree.put("merkleroot", encode_hash(header.merkle_root()));
    tree.put("time", header.timestamp());
    tree.put("nonce", header.nonce());
    tree.put("bits", hex(header.bits()));
    return to_json(tree, id);
}

std::string to_json(const chain::block& block, uint32_t id)
{
    return to_json(property_tree(block, true), id);
}

std::string to_json(const chain::block& block, uint32_t, uint32_t id)
{
    return to_json(block.header(), id);
}

std::string to_json(const chain::transaction& transaction, uint32_t id)
{
    return to_json(property_tree(config::transaction(transaction), true), id);
}

} // namespace rpc


} // namespace http
} // namespace protocol
} // namespace libbitcoin

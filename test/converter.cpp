/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin-protocol.
 *
 * libbitcoin-protocol is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <memory>
#include <string>
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <bitcoin/protocol.hpp>

using namespace bc;
using namespace bc::chain;
using namespace bc::protocol;

#define BCP_GENESIS_BLOCK_HASH \
"000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"

#define BCP_SATOSHIS_WORDS_TX_HASH \
"4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"

std::string encoded_script = "76a91418c0bd8d1818f1bf99cb1df2269c645318ef7b7388ac\n";

// TODO: move these comparison operators to the bc::chain classes.
bool operator==(const chain::script& a, const chain::script& b)
{
    const auto encoded_a = a.to_data(false);
    const auto encoded_b = b.to_data(false);
    return encoded_a == encoded_b;
}

bool operator==(const chain::input& a, const chain::input& b)
{
    return (a.previous_output == b.previous_output)
        && (a.script == b.script) && (a.sequence == b.sequence);
}

bool operator==(const chain::output& a, const chain::output& b)
{
    return (a.value == b.value) && (a.script == b.script);
}

bool operator==(const chain::transaction& a, const chain::transaction& b)
{
    auto equal = (a.version == b.version) && (a.locktime == b.locktime);

    if (equal && (a.inputs.size() == b.inputs.size()))
        for (data_chunk::size_type i = 0; equal && i < a.inputs.size(); i++)
            equal = a.inputs[i] == b.inputs[i];

    if (equal && (a.outputs.size() == b.outputs.size()))
        for (data_chunk::size_type i = 0; equal && (i < a.outputs.size()); i++)
            equal = a.outputs[i] == b.outputs[i];

    return equal;
}

bool operator==(const chain::block& a, const chain::block& b)
{
    if (a.header != b.header || a.transactions.size() != b.transactions.size())
        return false;

    for (data_chunk::size_type i = 0; i < a.transactions.size(); i++)
        if (!(a.transactions[i] == b.transactions[i]))
            return false;

    return true;
}

BOOST_AUTO_TEST_SUITE(converter_tests)

BOOST_AUTO_TEST_CASE(roundtrip_point_valid)
{
    const output_point initial
    {
        hash_literal(BCP_GENESIS_BLOCK_HASH),
        154
    };

    converter converter;
    std::shared_ptr<protocol::point> intermediate(converter.to_protocol(initial));
    BOOST_REQUIRE(intermediate.get() != nullptr);

    output_point result;
    BOOST_REQUIRE(converter.from_protocol(intermediate, result));
    BOOST_REQUIRE(initial == result);
}

BOOST_AUTO_TEST_CASE(roundtrip_transaction_input_valid)
{
    chain::script script_instance;
    const data_chunk data(encoded_script.begin(), encoded_script.end());
    BOOST_REQUIRE(script_instance.from_data(data, false, chain::script::parse_mode::raw_data_fallback));

    const chain::input initial
    {
        { hash_literal(BCP_GENESIS_BLOCK_HASH), 154 },
        script_instance, 64724
    };

    converter converter;
    std::shared_ptr<protocol::tx_input> intermediate(converter.to_protocol(initial));
    BOOST_REQUIRE(intermediate.get() != nullptr);

    input result;
    BOOST_REQUIRE_EQUAL(true, converter.from_protocol(intermediate, result));
    BOOST_REQUIRE(initial == result);
}

BOOST_AUTO_TEST_CASE(roundtrip_transaction_output_valid)
{
    chain::script script_instance;
    const data_chunk data(encoded_script.begin(), encoded_script.end());
    BOOST_REQUIRE(script_instance.from_data(data, false, chain::script::parse_mode::raw_data_fallback));

    converter converter;
    chain::output initial{ 6548621547, script_instance };
    std::shared_ptr<protocol::tx_output> intermediate(converter.to_protocol(initial));

    BOOST_REQUIRE(intermediate.get() != nullptr);

    chain::output result;
    BOOST_REQUIRE_EQUAL(true, converter.from_protocol(intermediate, result));
    BOOST_REQUIRE(initial == result);
}

BOOST_AUTO_TEST_CASE(roundtrip_transaction_valid)
{
    chain::script script_instance;
    const data_chunk data(encoded_script.begin(), encoded_script.end());
    BOOST_REQUIRE(script_instance.from_data(data, false, chain::script::parse_mode::raw_data_fallback));

    const chain::input::list tx_inputs
    {
        {
            { hash_literal(BCP_GENESIS_BLOCK_HASH), 154 },
            script_instance,
            64724
        }
    };
    const chain::output::list tx_outputs { { 6548621547, script_instance } };
    const chain::transaction initial{ 481547, 235123, tx_inputs, tx_outputs };

    converter converter;
    std::shared_ptr<protocol::tx> intermediate(converter.to_protocol(initial));
    BOOST_REQUIRE(intermediate);

    chain::transaction result;
    BOOST_REQUIRE_EQUAL(true, converter.from_protocol(intermediate, result));
    BOOST_REQUIRE(initial == result);
}

BOOST_AUTO_TEST_CASE(roundtrip_block_header_valid)
{
    const chain::header initial
    {
        6535,
        hash_literal(BCP_GENESIS_BLOCK_HASH),
        hash_literal(BCP_SATOSHIS_WORDS_TX_HASH),
        856345324,
        21324121,
        576859232,
        16
    };

    converter converter;
    std::shared_ptr<protocol::block_header> intermediate(converter.to_protocol(initial));
    BOOST_REQUIRE(intermediate);

    chain::header result;
    BOOST_REQUIRE_EQUAL(true, converter.from_protocol(intermediate, result));
    BOOST_REQUIRE(initial == result);
}

BOOST_AUTO_TEST_CASE(roundtrip_block_valid)
{
    chain::script script_instance;
    const data_chunk data(encoded_script.begin(), encoded_script.end());
    BOOST_REQUIRE(script_instance.from_data(data, false, chain::script::parse_mode::raw_data_fallback));

    const chain::input::list tx_inputs
    {
        {
            { hash_literal(BCP_GENESIS_BLOCK_HASH), 154 },
            script_instance,
            64724
        }
    };
    const chain::output::list tx_outputs{ chain::output{ 6548621547, script_instance } };
    const chain::transaction::list transactions{ { 481547, 235123, tx_inputs, tx_outputs } };
    const chain::header header
    {
        6535,
        hash_literal(BCP_GENESIS_BLOCK_HASH),
        hash_literal(BCP_SATOSHIS_WORDS_TX_HASH),
        856345324,
        21324121,
        576859232,
        1
    };
    const chain::block initial{ header, transactions };

    converter converter;
    std::shared_ptr<protocol::block> intermediate(converter.to_protocol(initial));
    BOOST_REQUIRE(intermediate);

    chain::block result;
    BOOST_REQUIRE_EQUAL(true, converter.from_protocol(intermediate, result));
    BOOST_REQUIRE(initial == result);
}

BOOST_AUTO_TEST_SUITE_END()

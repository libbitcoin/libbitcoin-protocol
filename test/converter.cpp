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
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <bitcoin/protocol.hpp>

#define BCP_GENESIS_BLOCK_HASH \
"000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"

#define BCP_SATOSHIS_WORDS_TX_HASH \
"4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"

std::string encoded_script
    = "76a91418c0bd8d1818f1bf99cb1df2269c645318ef7b7388ac\n";

bc::chain::script script_instance(
    bc::data_chunk(encoded_script.begin(), encoded_script.end()), true);

bool operator==(
    const bc::chain::script& a, const bc::chain::script& b)
{
    bc::data_chunk encoded_a = a;
    bc::data_chunk encoded_b = b;

    return (encoded_a == encoded_b);
}

bool operator==(
    const bc::chain::transaction_input& a,
    const bc::chain::transaction_input& b)
{
    return (a.previous_output == b.previous_output) && (a.script == b.script)
        && (a.sequence == b.sequence);
}

bool operator==(
    const bc::chain::transaction_output& a,
    const bc::chain::transaction_output& b)
{
    return (a.value == b.value) && (a.script == b.script);
}

bool operator==(
    const bc::chain::transaction& a, const bc::chain::transaction& b)
{
    bool equal = (a.version == b.version) && (a.locktime == b.locktime);

    if (equal && (a.inputs.size() == b.inputs.size()))
    {
        for (bc::data_chunk::size_type i = 0; equal && (i < a.inputs.size()); i++)
        {
            equal = (a.inputs[i] == b.inputs[i]);
        }
    }

    if (equal && (a.outputs.size() == b.outputs.size()))
    {
        for (bc::data_chunk::size_type i = 0; equal && (i < a.outputs.size()); i++)
        {
            equal = (a.outputs[i] == b.outputs[i]);
        }
    }

    return equal;
}

bool operator==(
    const bc::chain::block& a, const bc::chain::block& b)
{
    bool equal = (a.header == b.header);

    if (equal && (a.transactions.size() == b.transactions.size()))
    {
        for (bc::data_chunk::size_type i = 0; equal && (i < a.transactions.size()); i++)
        {
            equal = (a.transactions[i] == b.transactions[i]);
        }
    }

    return equal;
}

BOOST_AUTO_TEST_SUITE(converter_tests)

BOOST_AUTO_TEST_CASE(roundtrip_point_valid)
{
    bc::chain::output_point initial = {
        bc::hash_literal(BCP_GENESIS_BLOCK_HASH),
        154
    };

    bc::protocol::converter converter;

    std::shared_ptr<bc::protocol::point> intermediate(
        converter.to_protocol(initial));

    BOOST_REQUIRE(intermediate.get() != nullptr);

    bc::chain::output_point result;

    BOOST_REQUIRE_EQUAL(true, converter.from_protocol(intermediate, result));

    // verify roundtrip
    BOOST_REQUIRE(initial == result);
}

BOOST_AUTO_TEST_CASE(roundtrip_transaction_input_valid)
{
    bc::chain::transaction_input initial;
    initial.previous_output =  { bc::hash_literal(BCP_GENESIS_BLOCK_HASH), 154 };
    initial.script = script_instance;
    initial.sequence = 64724;

    bc::protocol::converter converter;

    std::shared_ptr<bc::protocol::tx_input> intermediate(
        converter.to_protocol(initial));

    BOOST_REQUIRE(intermediate.get() != nullptr);

    bc::chain::transaction_input result;

    BOOST_REQUIRE_EQUAL(true, converter.from_protocol(intermediate, result));

    // verify roundtrip
    BOOST_REQUIRE(initial == result);
}

BOOST_AUTO_TEST_CASE(roundtrip_transaction_output_valid)
{
    bc::chain::transaction_output initial;
    initial.script = script_instance;
    initial.value = 6548621547;

    bc::protocol::converter converter;

    std::shared_ptr<bc::protocol::tx_output> intermediate(
        converter.to_protocol(initial));

    BOOST_REQUIRE(intermediate.get() != nullptr);

    bc::chain::transaction_output result;

    BOOST_REQUIRE_EQUAL(true, converter.from_protocol(intermediate, result));

    // verify roundtrip
    BOOST_REQUIRE(initial == result);
}

BOOST_AUTO_TEST_CASE(roundtrip_transaction_valid)
{
    bc::chain::transaction_input tx_input;
    tx_input.previous_output = { bc::hash_literal(BCP_GENESIS_BLOCK_HASH), 154 };
    tx_input.script = script_instance;
    tx_input.sequence = 64724;

    bc::chain::transaction_output tx_output;
    tx_output.script = script_instance;
    tx_output.value = 6548621547;

    bc::chain::transaction initial;
    initial.version = 481547;
    initial.locktime = 235123;
    initial.inputs.push_back(tx_input);
    initial.outputs.push_back(tx_output);

    bc::protocol::converter converter;

    std::shared_ptr<bc::protocol::tx> intermediate(
        converter.to_protocol(initial));

    BOOST_REQUIRE(intermediate.get() != nullptr);

    bc::chain::transaction result;

    BOOST_REQUIRE_EQUAL(true, converter.from_protocol(intermediate, result));

    // verify roundtrip
    BOOST_REQUIRE(initial == result);
}

BOOST_AUTO_TEST_CASE(roundtrip_block_header_valid)
{
    bc::chain::block_header initial;
    initial.version = 6535;
    initial.previous_block_hash = bc::hash_literal(BCP_GENESIS_BLOCK_HASH);
    initial.merkle = bc::hash_literal(BCP_SATOSHIS_WORDS_TX_HASH);
    initial.timestamp = 856345324;
    initial.bits = 21324121;
    initial.nonce = 576859232;

    bc::protocol::converter converter;

    std::shared_ptr<bc::protocol::block_header> intermediate(
        converter.to_protocol(initial));

    BOOST_REQUIRE(intermediate.get() != nullptr);

    bc::chain::block_header result;

    BOOST_REQUIRE_EQUAL(true, converter.from_protocol(intermediate, result));

    // verify roundtrip
    BOOST_REQUIRE(initial == result);
}

BOOST_AUTO_TEST_CASE(roundtrip_block_valid)
{
    bc::chain::transaction_input tx_input;
    tx_input.previous_output = { bc::hash_literal(BCP_GENESIS_BLOCK_HASH), 154 };
    tx_input.script = script_instance;
    tx_input.sequence = 64724;

    bc::chain::transaction_output tx_output;
    tx_output.script = script_instance;
    tx_output.value = 6548621547;

    bc::chain::transaction tx;
    tx.version = 481547;
    tx.locktime = 235123;
    tx.inputs.push_back(tx_input);
    tx.outputs.push_back(tx_output);

    bc::chain::block initial;
    initial.header.version = 6535;
    initial.header.previous_block_hash = bc::hash_literal(BCP_GENESIS_BLOCK_HASH);
    initial.header.merkle = bc::hash_literal(BCP_SATOSHIS_WORDS_TX_HASH);
    initial.header.timestamp = 856345324;
    initial.header.bits = 21324121;
    initial.header.nonce = 576859232;

    initial.transactions.push_back(tx);

    bc::protocol::converter converter;

    std::shared_ptr<bc::protocol::block> intermediate(
        converter.to_protocol(initial));

    BOOST_REQUIRE(intermediate.get() != nullptr);

    bc::chain::block result;

    BOOST_REQUIRE_EQUAL(true, converter.from_protocol(intermediate, result));

    // verify roundtrip
    BOOST_REQUIRE(initial == result);
}

BOOST_AUTO_TEST_SUITE_END()

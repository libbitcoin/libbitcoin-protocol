/*
 * Copyright (c) 2011-2014 libbitcoin developers (see AUTHORS)
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

static bc::hash_digest hash_literal(const std::string& in)
{
    bc::hash_digest out;
    BITCOIN_ASSERT(bc::decode_hash(out, in));
    return out;
}

#define BCP_GENESIS_BLOCK_HASH \
"000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"

#define BCP_SATOSHIS_WORDS_TX_HASH \
"4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"

std::string encoded_script
    = "76a91418c0bd8d1818f1bf99cb1df2269c645318ef7b7388ac\n";

bool operator==(
    const bc::script_type& a, const bc::script_type& b)
{
    return (bc::save_script(a) == bc::save_script(b));
}

bool operator==(
    const bc::transaction_input_type& a, const bc::transaction_input_type& b)
{
    return (a.previous_output == b.previous_output) && (a.script == b.script)
        && (a.sequence == b.sequence);
}

bool operator==(
    const bc::transaction_output_type& a, const bc::transaction_output_type& b)
{
    return (a.value == b.value) && (a.script == b.script);
}

bool operator==(
    const bc::transaction_type& a, const bc::transaction_type& b)
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
    const bc::block_type& a, const bc::block_type& b)
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
    bc::output_point initial = { hash_literal(BCP_GENESIS_BLOCK_HASH), 154 };

    bc::protocol::converter converter;

    std::shared_ptr<bc::protocol::point> intermediate(
        converter.to_protocol(initial));

    BOOST_REQUIRE(intermediate.get() != nullptr);

    bc::output_point result;

    BOOST_REQUIRE_EQUAL(true, converter.from_protocol(intermediate, result));

    // verify roundtrip
    BOOST_REQUIRE(initial == result);
}

BOOST_AUTO_TEST_CASE(roundtrip_transaction_input_valid)
{
    bc::transaction_input_type initial = {
        { hash_literal(BCP_GENESIS_BLOCK_HASH), 154 },
        bc::raw_data_script(
            bc::data_chunk(encoded_script.begin(), encoded_script.end())),
        64724
    };

    bc::protocol::converter converter;

    std::shared_ptr<bc::protocol::tx_input> intermediate(
        converter.to_protocol(initial));

    BOOST_REQUIRE(intermediate.get() != nullptr);

    bc::transaction_input_type result;

    BOOST_REQUIRE_EQUAL(true, converter.from_protocol(intermediate, result));

    // verify roundtrip
    BOOST_REQUIRE(initial == result);
}

BOOST_AUTO_TEST_CASE(roundtrip_transaction_output_valid)
{
    bc::transaction_output_type initial = {
        6548621547,
        bc::raw_data_script(
            bc::data_chunk(encoded_script.begin(), encoded_script.end())),
    };

    bc::protocol::converter converter;

    std::shared_ptr<bc::protocol::tx_output> intermediate(
        converter.to_protocol(initial));

    BOOST_REQUIRE(intermediate.get() != nullptr);

    bc::transaction_output_type result;

    BOOST_REQUIRE_EQUAL(true, converter.from_protocol(intermediate, result));

    // verify roundtrip
    BOOST_REQUIRE(initial == result);
}

BOOST_AUTO_TEST_CASE(roundtrip_transaction_valid)
{
    bc::transaction_type initial = {
        481547,
        235123,
        {
            {
                { hash_literal(BCP_GENESIS_BLOCK_HASH), 154 },
                bc::raw_data_script(
                    bc::data_chunk(
                        encoded_script.begin(), encoded_script.end())),
                    64724
            }
        },
        {
            {
                6548621547,
                bc::raw_data_script(
                    bc::data_chunk(
                        encoded_script.begin(), encoded_script.end())),
            }
        }
    };

    bc::protocol::converter converter;

    std::shared_ptr<bc::protocol::tx> intermediate(
        converter.to_protocol(initial));

    BOOST_REQUIRE(intermediate.get() != nullptr);

    bc::transaction_type result;

    BOOST_REQUIRE_EQUAL(true, converter.from_protocol(intermediate, result));

    // verify roundtrip
    BOOST_REQUIRE(initial == result);
}

BOOST_AUTO_TEST_CASE(roundtrip_block_header_valid)
{
    bc::block_header_type initial = {
        6535,
        hash_literal(BCP_GENESIS_BLOCK_HASH),
        hash_literal(BCP_SATOSHIS_WORDS_TX_HASH),
        856345324,
        21324121,
        576859232
    };

    bc::protocol::converter converter;

    std::shared_ptr<bc::protocol::block_header> intermediate(
        converter.to_protocol(initial));

    BOOST_REQUIRE(intermediate.get() != nullptr);

    bc::block_header_type result;

    BOOST_REQUIRE_EQUAL(true, converter.from_protocol(intermediate, result));

    // verify roundtrip
    BOOST_REQUIRE(initial == result);
}

BOOST_AUTO_TEST_CASE(roundtrip_block_valid)
{
    bc::block_type initial = {
        {
            6535,
            hash_literal(BCP_GENESIS_BLOCK_HASH),
            hash_literal(BCP_SATOSHIS_WORDS_TX_HASH),
            856345324,
            21324121,
            576859232
        },
        {
            {
                481547,
                235123,
                {
                    {
                        { hash_literal(BCP_GENESIS_BLOCK_HASH), 154 },
                        bc::raw_data_script(
                            bc::data_chunk(
                                encoded_script.begin(), encoded_script.end())),
                            64724
                    }
                },
                {
                    {
                        6548621547,
                        bc::raw_data_script(
                            bc::data_chunk(
                                encoded_script.begin(), encoded_script.end())),
                    }
                }
            }
        }
    };

    bc::protocol::converter converter;

    std::shared_ptr<bc::protocol::block> intermediate(
        converter.to_protocol(initial));

    BOOST_REQUIRE(intermediate.get() != nullptr);

    bc::block_type result;

    BOOST_REQUIRE_EQUAL(true, converter.from_protocol(intermediate, result));

    // verify roundtrip
    BOOST_REQUIRE(initial == result);
}

BOOST_AUTO_TEST_SUITE_END()

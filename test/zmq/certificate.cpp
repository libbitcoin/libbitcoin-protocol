/**
 * Copyright (c) 2016 libbitcoin developers (see AUTHORS)
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

using namespace bc;
using namespace bc::protocol::zmq;

// These tests don't validate calls to zmq_curve_keypair or zmq_curve_public.
// These should be moved to trivial virtual methods for test isolation.

BOOST_AUTO_TEST_SUITE(certificate_tests)

static std::string base85_alphabet = 
    "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    ".-:+=^!/*?&<>()[]{}@%$#";


static const std::string null_private_key = "0000000000000000000000000000000000000000";
static const std::string null_public_key = "fxERSn6LHg6!4!qu+m-(f-Q<1UF!=7)u0-ysJ-^F";

static inline bool is_valid(const std::string& key, bool setting)
{
    for (const auto letter: key)
    {
        if (setting && letter == '#')
            return false;

        if (base85_alphabet.find_first_of(letter) == std::string::npos)
            return false;
    }

    return key.size() == 40;
}

class certificate_fixture
   : public certificate
{
public:
    static bool derive(std::string& out_public, const std::string& private_key)
    {
        return certificate::derive(out_public, private_key);
    }

    static bool create(std::string& out_public, std::string& out_private,
        bool setting)
    {
        return certificate::create(out_public, out_private, setting);
    }
};

BOOST_AUTO_TEST_CASE(certificate__construct1__default__creates_valid_keypair)
{
    certificate instance;
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(is_valid(instance.public_key(), false));
    BOOST_REQUIRE(is_valid(instance.private_key(), false));
}

BOOST_AUTO_TEST_CASE(certificate__construct1__setting__creates_valid_keypair)
{
    certificate instance(true);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(is_valid(instance.public_key(), true));
    BOOST_REQUIRE(is_valid(instance.private_key(), true));
}

BOOST_AUTO_TEST_CASE(certificate__construct2__null_hash__expected_keypair)
{
    certificate instance(null_hash);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE_EQUAL(instance.public_key(), null_public_key);
    BOOST_REQUIRE_EQUAL(instance.private_key(), null_private_key);
}

BOOST_AUTO_TEST_CASE(certificate__construct3__null_key_string__expected_keypair)
{
    certificate instance(null_private_key);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE_EQUAL(instance.public_key(), null_public_key);
    BOOST_REQUIRE_EQUAL(instance.private_key(), null_private_key);
}

BOOST_AUTO_TEST_CASE(certificate__construct3__bogus_invalid_and_empty_keypair)
{
    certificate instance(std::string("bogus"));
    BOOST_REQUIRE(!instance);
    BOOST_REQUIRE_EQUAL(instance.public_key(), "");
    BOOST_REQUIRE_EQUAL(instance.private_key(), "");
}

BOOST_AUTO_TEST_CASE(certificate__derive__bogus__false_value_unchanged)
{
    const auto expected = "42";
    std::string out_public = expected;
    BOOST_REQUIRE(!certificate_fixture::derive(out_public, "bogus"));
    BOOST_REQUIRE_EQUAL(out_public, expected);
}

BOOST_AUTO_TEST_CASE(certificate__derive__null_string__expected_public_key)
{
    std::string out_public;
    BOOST_REQUIRE(certificate_fixture::derive(out_public, null_private_key));
    BOOST_REQUIRE_EQUAL(out_public, null_public_key);
}

BOOST_AUTO_TEST_CASE(certificate__create__default__valid_keypair)
{
    std::string out_public;
    std::string out_private;
    BOOST_REQUIRE(certificate_fixture::create(out_public, out_private, false));
    BOOST_REQUIRE(is_valid(out_public, false));
    BOOST_REQUIRE(is_valid(out_private, false));
}

BOOST_AUTO_TEST_CASE(certificate__create__setting__valid_keypair)
{
    std::string out_public;
    std::string out_private;
    BOOST_REQUIRE(certificate_fixture::create(out_public, out_private, true));
    BOOST_REQUIRE(is_valid(out_public, true));
    BOOST_REQUIRE(is_valid(out_private, true));
}

BOOST_AUTO_TEST_SUITE_END()

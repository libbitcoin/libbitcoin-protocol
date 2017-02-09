/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <bitcoin/protocol.hpp>

using namespace bc;
using namespace bc::config;
using namespace bc::protocol::zmq;

// These tests don't validate calls to zmq_curve_keypair or zmq_curve_public.
// These should be moved to trivial virtual methods for test isolation.

BOOST_AUTO_TEST_SUITE(certificate_tests)

#define PRIVATE_KEY "JTKVSB%%)wK0E.X)V>+}o?pNmC{O&4W4b!Ni{Lh6"
#define PUBLIC_KEY "rq:rM>}U?@Lns47E1%kR.o@n%FcmmsL/@{H8]yf7"

// Determine if the key is valid as indicated by the setting option.
static inline bool is_valid(const config::sodium& key, bool setting)
{
    if (!key)
        return false;

    for (const auto letter: key.to_string())
        if (setting && letter == '#')
            return false;

    return true;
}

// Access protected members.
class certificate_fixture
   : public certificate
{
public:
    static bool derive(config::sodium& out_public,
        const config::sodium& private_key)
    {
        return certificate::derive(out_public, private_key);
    }

    static bool create(config::sodium& out_public, config::sodium& out_private,
        bool setting)
    {
        return certificate::create(out_public, out_private, setting);
    }
};

BOOST_AUTO_TEST_CASE(certificate__construct1__default__creates_valid_keypair)
{
    const certificate instance;
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(is_valid(instance.public_key(), true));
    BOOST_REQUIRE(is_valid(instance.private_key(), true));
}

BOOST_AUTO_TEST_CASE(certificate__construct2__null_hash_private_key__creates_valid_keypair)
{
    const sodium key(null_hash);
    const certificate instance(key);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(is_valid(instance.public_key(), false));
    BOOST_REQUIRE(is_valid(instance.private_key(), false));
}

BOOST_AUTO_TEST_CASE(certificate__construct2__valid_private_key__derives_expected_keypair)
{
    const sodium key(PRIVATE_KEY);
    const certificate instance(key);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE_EQUAL(instance.private_key().to_string(), PRIVATE_KEY);
    BOOST_REQUIRE_EQUAL(instance.public_key().to_string(), PUBLIC_KEY);
}

BOOST_AUTO_TEST_CASE(certificate__derive__null_hash__false)
{
    config::sodium out_public;
    BOOST_REQUIRE(!certificate_fixture::derive(out_public, {}));
}

BOOST_AUTO_TEST_CASE(certificate__derive__valid__true_expected_public_key)
{
    config::sodium out_public;
    BOOST_REQUIRE(certificate_fixture::derive(out_public, { PRIVATE_KEY }));
    BOOST_REQUIRE_EQUAL(out_public.to_string(), PUBLIC_KEY);
}

BOOST_AUTO_TEST_CASE(certificate__create__default__valid_keypair)
{
    config::sodium out_public;
    config::sodium out_private;
    BOOST_REQUIRE(certificate_fixture::create(out_public, out_private, false));
    BOOST_REQUIRE(is_valid(out_public, false));
    BOOST_REQUIRE(is_valid(out_private, false));
}

BOOST_AUTO_TEST_CASE(certificate__create__setting__valid_keypair)
{
    config::sodium out_public;
    config::sodium out_private;
    BOOST_REQUIRE(certificate_fixture::create(out_public, out_private, true));
    BOOST_REQUIRE(is_valid(out_public, true));
    BOOST_REQUIRE(is_valid(out_private, true));
}

BOOST_AUTO_TEST_SUITE_END()

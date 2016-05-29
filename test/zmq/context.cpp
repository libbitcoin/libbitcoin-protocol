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

using namespace bc::protocol::zmq;

// These tests don't validate calls to zmq_ctx_new or zmq_ctx_term.
// These should be moved to trivial virtual methods for test isolation.

BOOST_AUTO_TEST_SUITE(context_tests)

BOOST_AUTO_TEST_CASE(context__constructor__always__creates_invalid_instance)
{
    context instance;
    BOOST_REQUIRE(!instance);
}

BOOST_AUTO_TEST_CASE(context__start__always__sets_invalid_instance)
{
    context instance;
    instance.start();
    BOOST_REQUIRE(instance);
}

BOOST_AUTO_TEST_CASE(context__stop__always__invalidates_instance)
{
    context instance;
    instance.start();
    instance.stop();
    BOOST_REQUIRE(!instance);
}

BOOST_AUTO_TEST_CASE(context__self__valid_instance__is_not_null)
{
    context instance;
    instance.start();
    BOOST_REQUIRE(instance.self() != nullptr);
}

BOOST_AUTO_TEST_CASE(context__self__invalid_instance__is_null)
{
    context instance;
    BOOST_REQUIRE(instance.self() == nullptr);
}

BOOST_AUTO_TEST_SUITE_END()

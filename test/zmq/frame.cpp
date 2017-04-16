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
using namespace bc::protocol::zmq;

BOOST_AUTO_TEST_SUITE(frame_tests)

// constuctor1

BOOST_AUTO_TEST_CASE(frame__constuctor1__always__valid)
{
    const frame instance;
    BOOST_REQUIRE(instance);
}

BOOST_AUTO_TEST_CASE(frame__constuctor1__always__no_more)
{
    const frame instance;
    BOOST_REQUIRE(!instance.more());
}

BOOST_AUTO_TEST_CASE(frame__constuctor1__always__empty_payload)
{
    const frame instance;
    BOOST_REQUIRE(instance.payload().empty());
}

// constuctor2

BOOST_AUTO_TEST_CASE(frame__constuctor2__empty__valid)
{
    const frame instance{ data_chunk{} };
    BOOST_REQUIRE(instance);
}

BOOST_AUTO_TEST_CASE(frame__constuctor2__empty__no_more)
{
    const frame instance{ data_chunk{} };
    BOOST_REQUIRE(!instance.more());
}

BOOST_AUTO_TEST_CASE(frame__constuctor2__empty__empty_payload)
{
    const frame instance{ data_chunk{} };
    BOOST_REQUIRE(instance.payload().empty());
}

BOOST_AUTO_TEST_CASE(frame__constuctor2__non_empty__valid)
{
    static const data_chunk expected{ 0xba, 0xad, 0xf0, 0x0d };
    const frame instance{ expected };
    BOOST_REQUIRE(instance);
}

BOOST_AUTO_TEST_CASE(frame__constuctor2__non_empty__no_more)
{
    static const data_chunk expected{ 0xba, 0xad, 0xf0, 0x0d };
    const frame instance{ expected };
    BOOST_REQUIRE(!instance.more());
}

BOOST_AUTO_TEST_CASE(frame__constuctor2__non_empty__expected_payload)
{
    static const data_chunk expected{ 0xba, 0xad, 0xf0, 0x0d };
    const frame instance{ expected };
    BOOST_REQUIRE(instance.payload() == expected);
}

BOOST_AUTO_TEST_SUITE_END()

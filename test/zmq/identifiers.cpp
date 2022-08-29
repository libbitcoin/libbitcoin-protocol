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
#include "../test.hpp"

BOOST_AUTO_TEST_SUITE(identifiers_tests)

using namespace bc::protocol::zmq;

class identifiers_fixture
  : public identifiers
{
public:
    void push(const void* socket) override
    {
        identifiers::push(socket);
    }
};

BOOST_AUTO_TEST_CASE(identifiers__empty__default__true)
{
    identifiers_fixture instance;
    BOOST_REQUIRE(instance.empty());
}

BOOST_AUTO_TEST_CASE(identifiers__empty__one_value__false)
{
    identifiers_fixture instance;
    instance.push(reinterpret_cast<void*>(42u));
    BOOST_REQUIRE(!instance.empty());
}

BOOST_AUTO_TEST_CASE(identifiers__contains__default__not_zero)
{
    identifiers_fixture instance;
    BOOST_REQUIRE(!instance.contains(0));
}

BOOST_AUTO_TEST_CASE(identifiers__contains__one_value__expected)
{
    const intptr_t expected = 42u;
    identifiers_fixture instance;
    instance.push(reinterpret_cast<void*>(expected));
    BOOST_REQUIRE(instance.contains(expected));
}

BOOST_AUTO_TEST_CASE(identifiers__contains__two_values__expected)
{
    const intptr_t expected1 = 99u;
    const intptr_t expected2 = 42u;
    identifiers_fixture instance;
    instance.push(reinterpret_cast<void*>(expected1));
    instance.push(reinterpret_cast<void*>(expected2));
    BOOST_REQUIRE(instance.contains(expected1));
    BOOST_REQUIRE(instance.contains(expected2));
}

BOOST_AUTO_TEST_SUITE_END()

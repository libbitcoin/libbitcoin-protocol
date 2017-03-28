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

using namespace bc::protocol::zmq;

BOOST_AUTO_TEST_SUITE(message_tests)

static const bc::data_chunk chunk1{ 0xf0, 0x0d };
static const bc::data_chunk chunk2{ 0xba, 0xad, 0xf0, 0x0d };
static const auto number2 = bc::from_little_endian_unsafe<uint32_t>(chunk2.begin());
static const auto hash1 = bc::hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
static const auto hash2 = bc::hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");

class message_fixture
  : public message
{
public:
    bc::data_queue& queue()
    {
        return queue_;
    };
};

// constructor

BOOST_AUTO_TEST_CASE(message__constructor__always__empty)
{
    message instance;
    BOOST_REQUIRE(instance.empty());
}

BOOST_AUTO_TEST_CASE(message__constructor__always__size_0)
{
    message instance;
    BOOST_REQUIRE_EQUAL(instance.size(), 0u);
}

// enqueue1

BOOST_AUTO_TEST_CASE(message__enqueue1__empty__size_1)
{
    message instance;
    instance.enqueue();
    BOOST_REQUIRE(!instance.empty());
    BOOST_REQUIRE_EQUAL(instance.size(), 1u);
}

BOOST_AUTO_TEST_CASE(message__enqueue1__nonempty__ordered)
{
    message_fixture instance;
    instance.queue().emplace(chunk1);
    instance.enqueue();
    BOOST_REQUIRE_EQUAL(instance.size(), 2u);
    BOOST_REQUIRE(instance.queue().front() == chunk1);
    BOOST_REQUIRE(instance.queue().back().empty());
}

// enqueue2

BOOST_AUTO_TEST_CASE(message__enqueue2__empty__size_1_expected)
{
    message_fixture instance;
    instance.enqueue(chunk1);
    BOOST_REQUIRE_EQUAL(instance.size(), 1u);
    BOOST_REQUIRE(instance.queue().front() == chunk1);
}

BOOST_AUTO_TEST_CASE(message__enqueue2__nonempty__ordered)
{
    message_fixture instance;
    instance.queue().emplace(chunk1);
    instance.enqueue(chunk2);
    BOOST_REQUIRE_EQUAL(instance.size(), 2u);
    BOOST_REQUIRE(instance.queue().front() == chunk1);
    BOOST_REQUIRE(instance.queue().back() == chunk2);
}

// enqueue3

BOOST_AUTO_TEST_CASE(message__enqueue3__empty__size_1_expected)
{
    message_fixture instance;
    instance.enqueue(bc::to_chunk(chunk1));
    BOOST_REQUIRE_EQUAL(instance.size(), 1u);
    BOOST_REQUIRE(instance.queue().front() == chunk1);
}

BOOST_AUTO_TEST_CASE(message__enqueue3__nonempty__ordered)
{
    message_fixture instance;
    instance.queue().emplace(chunk1);
    instance.enqueue(bc::to_chunk(chunk2));
    BOOST_REQUIRE_EQUAL(instance.size(), 2u);
    BOOST_REQUIRE(instance.queue().front() == chunk1);
    BOOST_REQUIRE(instance.queue().back() == chunk2);
}

// enqueue4

BOOST_AUTO_TEST_CASE(message__enqueue4__empty__size_1_expected)
{
    // Ensure we aren't capturing the null terminator.
    const std::string text2(chunk2.begin(), chunk2.end());
    BOOST_REQUIRE_EQUAL(text2.size(), 4u);

    message_fixture instance;
    instance.enqueue(text2);
    BOOST_REQUIRE_EQUAL(instance.size(), 1u);
    const auto& value = instance.queue().front();
    BOOST_REQUIRE(value == chunk2);
}

BOOST_AUTO_TEST_CASE(message__enqueue4__nonempty__ordered)
{
    // Ensure we aren't capturing the null terminator.
    const std::string text2(chunk2.begin(), chunk2.end());
    BOOST_REQUIRE_EQUAL(text2.size(), 4u);

    message_fixture instance;
    instance.queue().emplace(chunk1);
    instance.enqueue(text2);
    BOOST_REQUIRE_EQUAL(instance.size(), 2u);
    BOOST_REQUIRE(instance.queue().front() == chunk1);
    BOOST_REQUIRE(instance.queue().back() == chunk2);
}

// enqueue_little_endian

BOOST_AUTO_TEST_CASE(message__enqueue_little_endian__empty__size_1_expected)
{
    message_fixture instance;
    instance.enqueue_little_endian<uint32_t>(number2);
    BOOST_REQUIRE_EQUAL(instance.size(), 1u);
    const auto bytes = instance.queue().front().begin();
    BOOST_REQUIRE_EQUAL(bc::from_little_endian_unsafe<uint32_t>(bytes), number2);
}

BOOST_AUTO_TEST_CASE(message__enqueue_little_endian__nonempty__ordered)
{
    message_fixture instance;
    instance.queue().emplace(chunk1);
    instance.enqueue_little_endian<uint32_t>(number2);
    BOOST_REQUIRE_EQUAL(instance.size(), 2u);
    BOOST_REQUIRE(instance.queue().front() == chunk1);
    BOOST_REQUIRE(instance.queue().back() == chunk2);
}

// clear

BOOST_AUTO_TEST_CASE(message__clear__empty__empty)
{
    message instance;
    instance.clear();
    BOOST_REQUIRE(instance.empty());
}

BOOST_AUTO_TEST_CASE(message__clear__nonempty__empty)
{
    message instance;
    instance.enqueue();
    instance.enqueue();
    instance.clear();
    BOOST_REQUIRE(instance.empty());
}

// dequeue1

BOOST_AUTO_TEST_CASE(message__dequeue1__empty__false)
{
    message instance;
    BOOST_REQUIRE(!instance.dequeue());
}

BOOST_AUTO_TEST_CASE(message__dequeue1__nonempty__true_empty)
{
    message_fixture instance;
    instance.queue().emplace(chunk1);
    BOOST_REQUIRE(instance.dequeue());
    BOOST_REQUIRE(instance.empty());
}

// dequeue2

BOOST_AUTO_TEST_CASE(message__dequeue2__empty__false)
{
    message_fixture instance;
    uint32_t out;
    BOOST_REQUIRE(!instance.dequeue(out));
}

BOOST_AUTO_TEST_CASE(message__dequeue2__mismatched__false_empty)
{
    message_fixture instance;
    instance.queue().emplace(chunk1);
    uint32_t out;
    BOOST_REQUIRE(!instance.dequeue(out));
    BOOST_REQUIRE(instance.empty());
}

BOOST_AUTO_TEST_CASE(message__dequeue2__two__true_ordered_expected)
{
    message_fixture instance;
    instance.queue().emplace(chunk2);
    instance.queue().emplace(chunk1);
    uint32_t out;
    BOOST_REQUIRE(instance.dequeue(out));
    BOOST_REQUIRE_EQUAL(out, number2);
}

// dequeue3

BOOST_AUTO_TEST_CASE(message__dequeue3__empty__false)
{
    message instance;
    bc::data_chunk out;
    BOOST_REQUIRE(!instance.dequeue(out));
}

BOOST_AUTO_TEST_CASE(message__dequeue3__two__true_ordered_expected)
{
    message_fixture instance;
    instance.queue().emplace(chunk2);
    instance.queue().emplace(chunk1);
    bc::data_chunk out;
    BOOST_REQUIRE(instance.dequeue(out));
    BOOST_REQUIRE(out == chunk2);
    BOOST_REQUIRE(instance.dequeue(out));
    BOOST_REQUIRE(out == chunk1);
}

// dequeue4

BOOST_AUTO_TEST_CASE(message__dequeue4__empty__false)
{
    message instance;
    std::string out;
    BOOST_REQUIRE(!instance.dequeue(out));
}

BOOST_AUTO_TEST_CASE(message__dequeue4__two__true_ordered_expected)
{
    message_fixture instance;
    instance.queue().emplace(chunk2);
    instance.queue().emplace(chunk1);
    std::string out;
    BOOST_REQUIRE(instance.dequeue(out));
    BOOST_REQUIRE(bc::to_chunk(out) == chunk2);
    BOOST_REQUIRE(instance.dequeue(out));
    BOOST_REQUIRE(bc::to_chunk(out) == chunk1);
}

// dequeue5

BOOST_AUTO_TEST_CASE(message__dequeue5__empty__false)
{
    message instance;
    bc::hash_digest out;
    BOOST_REQUIRE(!instance.dequeue(out));
}

BOOST_AUTO_TEST_CASE(message__dequeue5__mismatched__false_empty)
{
    message_fixture instance;
    instance.queue().emplace(chunk1);
    bc::hash_digest out;
    BOOST_REQUIRE(!instance.dequeue(out));
    BOOST_REQUIRE(instance.empty());
}

BOOST_AUTO_TEST_CASE(message__dequeue5__two__true_ordered_expected)
{
    message_fixture instance;
    instance.queue().emplace(bc::to_chunk(hash1));
    instance.queue().emplace(bc::to_chunk(hash2));
    bc::hash_digest out;
    BOOST_REQUIRE(instance.dequeue(out));
    BOOST_REQUIRE(out == hash1);
    BOOST_REQUIRE(instance.dequeue(out));
    BOOST_REQUIRE(out == hash2);
}

// dequeue_data

BOOST_AUTO_TEST_CASE(message__dequeue_data__empty__empty)
{
    message instance;
    BOOST_REQUIRE(instance.dequeue_data().empty());
}

BOOST_AUTO_TEST_CASE(message__dequeue_data__two__ordered_expected)
{
    message_fixture instance;
    instance.queue().emplace(chunk2);
    instance.queue().emplace(chunk1);
    BOOST_REQUIRE(instance.dequeue_data() == chunk2);
    BOOST_REQUIRE(instance.dequeue_data() == chunk1);
}

// dequeue_text

BOOST_AUTO_TEST_CASE(message__dequeue_text__from_empty__empty)
{
    message instance;
    BOOST_REQUIRE(instance.dequeue_text().empty());
}

BOOST_AUTO_TEST_CASE(message__dequeue_text__two__ordered_expected)
{
    message_fixture instance;
    instance.queue().emplace(chunk2);
    instance.queue().emplace(chunk1);
    BOOST_REQUIRE(bc::to_chunk(instance.dequeue_text()) == chunk2);
    BOOST_REQUIRE(bc::to_chunk(instance.dequeue_text()) == chunk1);
}

BOOST_AUTO_TEST_SUITE_END()

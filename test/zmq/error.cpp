/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(zmq_error_tests)

using namespace bc::protocol::zmq;

// error_t
// These test std::error_code equality operator overrides.

BOOST_AUTO_TEST_CASE(zmq_error_t__code__success__false_exected_message)
{
    constexpr auto value = error::success;
    const auto ec = error::code(value);
    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "success");
}

BOOST_AUTO_TEST_CASE(zmq_error_t__code__unknown__true_exected_message)
{
    constexpr auto value = error::unknown;
    const auto ec = error::code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "unknown error");
}

BOOST_AUTO_TEST_CASE(zmq_error_t__code__socket_state__true_exected_message)
{
    constexpr auto value = error::socket_state;
    const auto ec = error::code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "socket not in appropriate state");
}

BOOST_AUTO_TEST_CASE(zmq_error_t__code__context_terminated__true_exected_message)
{
    constexpr auto value = error::context_terminated;
    const auto ec = error::code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "socket context was terminated");
}

BOOST_AUTO_TEST_CASE(zmq_error_t__code__no_thread__true_exected_message)
{
    constexpr auto value = error::no_thread;
    const auto ec = error::code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "no thread available");
}

BOOST_AUTO_TEST_CASE(zmq_error_t__code__incompatible_protocol__true_exected_message)
{
    constexpr auto value = error::incompatible_protocol;
    const auto ec = error::code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "protocol incompatible with socket type");
}

BOOST_AUTO_TEST_CASE(zmq_error_t__code__host_unreachable__true_exected_message)
{
    constexpr auto value = error::host_unreachable;
    const auto ec = error::code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "host unreachable");
}

BOOST_AUTO_TEST_CASE(zmq_error_t__code__no_buffer_space__true_exected_message)
{
    constexpr auto value = error::no_buffer_space;
    const auto ec = error::code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "no buffer space available");
}

BOOST_AUTO_TEST_CASE(zmq_error_t__code__unsupported_operation__true_exected_message)
{
    constexpr auto value = error::unsupported_operation;
    const auto ec = error::code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "operation not supported");
}

BOOST_AUTO_TEST_CASE(zmq_error_t__code__unsupported_protocol__true_exected_message)
{
    constexpr auto value = error::unsupported_protocol;
    const auto ec = error::code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "protocol not supported");
}

BOOST_AUTO_TEST_CASE(zmq_error_t__code__network_down__true_exected_message)
{
    constexpr auto value = error::network_down;
    const auto ec = error::code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "network down");
}

BOOST_AUTO_TEST_CASE(zmq_error_t__code__address_in_use__true_exected_message)
{
    constexpr auto value = error::address_in_use;
    const auto ec = error::code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "address in use");
}

BOOST_AUTO_TEST_CASE(zmq_error_t__code__resolve_failed__true_exected_message)
{
    constexpr auto value = error::resolve_failed;
    const auto ec = error::code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "address resolution failed");
}

BOOST_AUTO_TEST_CASE(zmq_error_t__code__accept_failed__true_exected_message)
{
    constexpr auto value = error::accept_failed;
    const auto ec = error::code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "connection refused");
}

BOOST_AUTO_TEST_CASE(zmq_error_t__code__in_progress__true_exected_message)
{
    constexpr auto value = error::in_progress;
    const auto ec = error::code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "operation in progress");
}

BOOST_AUTO_TEST_CASE(zmq_error_t__code__try_again__true_exected_message)
{
    constexpr auto value = error::try_again;
    const auto ec = error::code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "non-blocking request but message cannot be sent now");
}

BOOST_AUTO_TEST_CASE(zmq_error_t__code__invalid_message__true_exected_message)
{
    constexpr auto value = error::invalid_message;
    const auto ec = error::code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "invalid message");
}

BOOST_AUTO_TEST_CASE(zmq_error_t__code__interrupted__true_exected_message)
{
    constexpr auto value = error::interrupted;
    const auto ec = error::code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "operation interrupted by signal before send");
}

BOOST_AUTO_TEST_CASE(zmq_error_t__code__invalid_socket__true_exected_message)
{
    constexpr auto value = error::invalid_socket;
    const auto ec = error::code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "invalid socket");
}

BOOST_AUTO_TEST_SUITE_END()

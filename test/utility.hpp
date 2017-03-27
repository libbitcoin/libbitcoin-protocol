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
#ifndef LIBBITCOIN_PROTOCOL_TEST_UTILITY_HPP
#define LIBBITCOIN_PROTOCOL_TEST_UTILITY_HPP

#define TEST_DOMAIN "testing"
#define TEST_MESSAGE "hello world!"
#define TEST_HOST "127.0.0.1"
#define TEST_HOST_BAD "127.0.0.42"
#define TEST_URL "tcp://" TEST_HOST ":9000"

#define BC_REQUIRE_SUCCESS(value) \
    BOOST_REQUIRE_EQUAL(value, error::success)

#define SEND_MESSAGE(socket) \
    zmq::message out__; \
    out__.enqueue(TEST_MESSAGE); \
    BC_REQUIRE_SUCCESS(socket.send(out__))

#define WAIT_SUCCESS(socket) \
    zmq::poller poller__; \
    poller__.add(socket); \
    while (!poller__.wait().contains(socket.id()))

#define WAIT_FAILURE(socket) \
    zmq::poller poller__; \
    poller__.add(socket); \
    BOOST_REQUIRE(!poller__.wait(1).contains(socket.id()))

#define RECEIVE_MESSAGE(socket) \
    zmq::message in__; \
    BC_REQUIRE_SUCCESS(socket.receive(in__)); \
    BOOST_REQUIRE_EQUAL(in__.dequeue_text(), TEST_MESSAGE)

#endif
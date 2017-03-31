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

#include <future>
#include <bitcoin/protocol.hpp>
#include "../utility.hpp"

using namespace bc;
using namespace bc::protocol;
using role = zmq::socket::role;

BOOST_AUTO_TEST_SUITE(socket_tests)

// There is no authenticator running, so this blocks despite configuration.
BOOST_AUTO_TEST_CASE(socket__push_pull__brickhouse__blocked)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::context context;
    BOOST_REQUIRE(context);

    zmq::socket pusher(context, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(pusher.set_private_key(server_certificate.private_key()));
    BOOST_REQUIRE(pusher.set_curve_server());
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_URL }));

    zmq::socket puller(context, role::puller);
    BOOST_REQUIRE(puller);
    BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    BOOST_REQUIRE(puller.set_certificate({}));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_URL }));

    SEND_MESSAGE(pusher);
    RECEIVE_FAILURE(puller);
}

// PUSH and PULL [asymmetrical, synchronous, unroutable]
BOOST_AUTO_TEST_CASE(socket__push_pull__grasslands__received)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    zmq::socket server(context, role::pusher);
    BOOST_REQUIRE(server);
    BC_REQUIRE_SUCCESS(server.bind({ TEST_URL }));

    zmq::socket client(context, role::puller);
    BOOST_REQUIRE(client);
    BC_REQUIRE_SUCCESS(client.connect({ TEST_URL }));

    SEND_MESSAGE(server);
    RECEIVE_MESSAGE(client);
}

// PAIR and PAIR [symmetrical, synchronous, unroutable]
BOOST_AUTO_TEST_CASE(socket__pair_pair__grasslands__received)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    zmq::socket server(context, role::pair);
    BOOST_REQUIRE(server);
    BC_REQUIRE_SUCCESS(server.bind({ TEST_URL }));

    zmq::socket client(context, role::pair);
    BOOST_REQUIRE(client);
    BC_REQUIRE_SUCCESS(client.connect({ TEST_URL }));

    SEND_MESSAGE(server);
    RECEIVE_MESSAGE(client);
}

// REQ and REP [asymetrical, synchronous, routable]
BOOST_AUTO_TEST_CASE(socket__requester_replier__grasslands__received)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    zmq::socket server(context, role::replier);
    BOOST_REQUIRE(server);
    BC_REQUIRE_SUCCESS(server.bind({ TEST_URL }));

    zmq::socket client(context, role::requester);
    BOOST_REQUIRE(client);
    BC_REQUIRE_SUCCESS(client.connect({ TEST_URL }));

    SEND_MESSAGE(client);
    RECEIVE_MESSAGE(server);
}

// PUB and SUB [asymmtrical, asynchronous, routable (subscription)]
BOOST_AUTO_TEST_CASE(socket__publisher_subscriber__grasslands_synchronous__missed)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    zmq::socket server(context, role::publisher);
    BOOST_REQUIRE(server);
    BC_REQUIRE_SUCCESS(server.bind({ TEST_URL }));

    zmq::socket client(context, role::subscriber);
    BOOST_REQUIRE(client);
    BC_REQUIRE_SUCCESS(client.connect({ TEST_URL }));

    // Because pub-sub is asynchronous, the receive misses the send.
    SEND_MESSAGE(server);
    RECEIVE_FAILURE(client);
}

BOOST_AUTO_TEST_CASE(socket__publisher_subscriber__grasslands_asynchronous__received)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    std::promise<bool> stopped;
    std::thread thread([&]()
    {
        zmq::socket client(context, role::subscriber);
        BOOST_REQUIRE(client);
        BC_REQUIRE_SUCCESS(client.connect({ TEST_URL }));

        RECEIVE_MESSAGE(client);
        client.stop();
        stopped.set_value(true);
    });

    zmq::socket server(context, role::publisher);
    BOOST_REQUIRE(server);
    BC_REQUIRE_SUCCESS(server.bind({ TEST_URL }));

    SEND_MESSAGES_UNTIL(server, stopped);
    thread.join();
}

// XPUB and XSUB are for routing PUB-SUB subscriptions
// ROUTER and DEALER are for routing REQ-REP messages

BOOST_AUTO_TEST_SUITE_END()

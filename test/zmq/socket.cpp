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

#include "../utility.hpp"
#include <bitcoin/protocol.hpp>

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
    pusher.stop();
    puller.stop();
}

// PUSH and PULL are asymmetrical, synchronous and not enveloped.
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
    server.stop();
    client.stop();
}

// PAIR is symmetrical, synchronous and not enveloped.
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
    server.stop();
    client.stop();
}

// REQ and REP are asymetrical, synchronous and enveloped.
// zguide.zeromq.org/page:all#The-Simple-Reply-Envelope
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
    client.stop();
    server.stop();
}

////// PUB and SUB are asymmtrical, asynchronous and not enveloped.
////BOOST_AUTO_TEST_CASE(socket__publisher_subscriber__grasslands__received)
////{
////    zmq::context context;
////    BOOST_REQUIRE(context);
////
////    zmq::socket server(context, role::publisher);
////    BOOST_REQUIRE(server);
////    BC_REQUIRE_SUCCESS(server.bind({ TEST_URL }));
////
////    zmq::socket client(context, role::subscriber);
////    BOOST_REQUIRE(client);
////    BC_REQUIRE_SUCCESS(client.connect({ TEST_URL }));
////
////    SEND_MESSAGE(server);
////
////    // TODO: use thread.
////    // Pub/Sub communication is asynchronous.
////    // If a “publish” service has been started already and then when you start
////    // “subscribe” service, it would not receive message already published. 
////    RECEIVE_MESSAGE(client);
////    server.stop();
////    client.stop();
////}

// XPUB and XSUB are for routing PUB-SUB subscriptions
// ROUTER and DEALER are for routing REQ-REP messages

BOOST_AUTO_TEST_SUITE_END()

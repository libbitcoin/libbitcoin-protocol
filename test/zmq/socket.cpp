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

#include <zmq.h>
#include <bitcoin/protocol.hpp>
#include "../utility.hpp"

using namespace bc;
using namespace bc::protocol;
using role = zmq::socket::role;

BOOST_AUTO_TEST_SUITE(socket_tests)

// See for zeromq curve pattern: hintjens.com/blog:49
// brickhouse = stonehouse - strawhouse (private and anonymous)

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
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(context, role::puller);
    BOOST_REQUIRE(puller);
    BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    BOOST_REQUIRE(puller.set_certificate({}));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_FAILURE(puller);
}

// PUSH and PULL [asymmetrical, synchronous, unroutable]
BOOST_AUTO_TEST_CASE(socket__push_pull__grasslands__received)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    zmq::socket pusher(context, role::pusher);
    BOOST_REQUIRE(pusher);
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(context, role::puller);
    BOOST_REQUIRE(puller);
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_MESSAGE(puller);
}

BOOST_AUTO_TEST_CASE(socket__push_pull__grasslands_connect_first__received)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    zmq::socket puller(context, role::puller);
    BOOST_REQUIRE(puller);
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket pusher(context, role::pusher);
    BOOST_REQUIRE(pusher);
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_MESSAGE(puller);
}

// PAIR and PAIR [symmetrical, synchronous, unroutable]
BOOST_AUTO_TEST_CASE(socket__pair_pair__grasslands__received)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    zmq::socket server(context, role::pair);
    BOOST_REQUIRE(server);
    BC_REQUIRE_SUCCESS(server.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket client(context, role::pair);
    BOOST_REQUIRE(client);
    BC_REQUIRE_SUCCESS(client.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(server);
    RECEIVE_MESSAGE(client);
}
BOOST_AUTO_TEST_CASE(socket__pair_pair__grasslands_connect_first__received)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    zmq::socket client(context, role::pair);
    BOOST_REQUIRE(client);
    BC_REQUIRE_SUCCESS(client.connect({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket server(context, role::pair);
    BOOST_REQUIRE(server);
    BC_REQUIRE_SUCCESS(server.bind({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(server);
    RECEIVE_MESSAGE(client);
}

// REQ and REP [asymetrical, synchronous, routable]
BOOST_AUTO_TEST_CASE(socket__req_rep__grasslands__received)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    zmq::socket replier(context, role::replier);
    BOOST_REQUIRE(replier);
    BC_REQUIRE_SUCCESS(replier.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket requester(context, role::requester);
    BOOST_REQUIRE(requester);
    BC_REQUIRE_SUCCESS(requester.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(requester);
    RECEIVE_MESSAGE(replier);
}

BOOST_AUTO_TEST_CASE(socket__req_rep__grasslands_connect_first__received)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    zmq::socket requester(context, role::requester);
    BOOST_REQUIRE(requester);
    BC_REQUIRE_SUCCESS(requester.connect({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket replier(context, role::replier);
    BOOST_REQUIRE(replier);
    BC_REQUIRE_SUCCESS(replier.bind({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(requester);
    RECEIVE_MESSAGE(replier);
}


// PUB and SUB [asymmtrical, asynchronous, routable (subscription)]
BOOST_AUTO_TEST_CASE(socket__pub_sub__grasslands_synchronous__missed)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    zmq::socket publisher(context, role::publisher);
    BOOST_REQUIRE(publisher);
    BC_REQUIRE_SUCCESS(publisher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket subscriber(context, role::subscriber);
    BOOST_REQUIRE(subscriber);
    BC_REQUIRE_SUCCESS(subscriber.connect({ TEST_PUBLIC_ENDPOINT }));

    // Because pub-sub is asynchronous, the receive misses the send.
    SEND_MESSAGE(publisher);
    RECEIVE_FAILURE(subscriber);
}

BOOST_AUTO_TEST_CASE(socket__pub_sub__grasslands_synchronous_connect_first__missed)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    zmq::socket subscriber(context, role::subscriber);
    BOOST_REQUIRE(subscriber);
    BC_REQUIRE_SUCCESS(subscriber.connect({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket publisher(context, role::publisher);
    BOOST_REQUIRE(publisher);
    BC_REQUIRE_SUCCESS(publisher.bind({ TEST_PUBLIC_ENDPOINT }));

    // The receiver is expected to miss initial sends even if already connected.
    SEND_MESSAGE(publisher);
    RECEIVE_FAILURE(subscriber);
}

BOOST_AUTO_TEST_CASE(socket__pub_sub__grasslands_asynchronous__received)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    zmq::socket publisher(context, role::publisher);
    BOOST_REQUIRE(publisher);
    BC_REQUIRE_SUCCESS(publisher.bind({ TEST_PUBLIC_ENDPOINT }));

    std::promise<bool> received;
    simple_thread subscriber_thread([&]()
    {
        zmq::socket subscriber(context, role::subscriber);
        BOOST_REQUIRE(subscriber);
        BC_REQUIRE_SUCCESS(subscriber.connect({ TEST_PUBLIC_ENDPOINT }));

        RECEIVE_MESSAGE(subscriber);
        received.set_value(true);
    });

    // We could use the poller, but this is a unit test.
    SEND_MESSAGES_UNTIL(publisher, received);
}

BOOST_AUTO_TEST_CASE(socket__pub_sub__grasslands_asynchronous_connect_first__received)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    std::promise<bool> received;
    simple_thread subscriber_thread([&]()
    {
        zmq::socket subscriber(context, role::subscriber);
        BOOST_REQUIRE(subscriber);
        BC_REQUIRE_SUCCESS(subscriber.connect({ TEST_PUBLIC_ENDPOINT }));

        RECEIVE_MESSAGE(subscriber);
        received.set_value(true);
    });

    zmq::socket publisher(context, role::publisher);
    BOOST_REQUIRE(publisher);
    BC_REQUIRE_SUCCESS(publisher.bind({ TEST_PUBLIC_ENDPOINT }));

    // We could use the poller, but this is a unit test.
    SEND_MESSAGES_UNTIL(publisher, received);
}

BOOST_AUTO_TEST_CASE(socket__xpub_xsub__grasslands_two_threads__subscribed)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    std::promise<bool> received;

    simple_thread publisher_thread([&]()
    {
        zmq::socket publisher(context, role::publisher);
        BOOST_REQUIRE(publisher);
        BC_REQUIRE_SUCCESS(publisher.bind({ TEST_PUBLIC_ENDPOINT }));

        SEND_MESSAGES_UNTIL(publisher, received);
    });

    simple_thread subscriber_thread([&]()
    {
        zmq::socket subscriber(context, role::subscriber);
        BOOST_REQUIRE(subscriber);
        BC_REQUIRE_SUCCESS(subscriber.connect({ TEST_PUBLIC_ENDPOINT }));

        RECEIVE_MESSAGE(subscriber);
        received.set_value(true);
    });
}

// XPUB and XSUB [subscription routing]
// See for espresso pattern: zguide.zeromq.org/c:espresso
// decaf = espresso - listener
BOOST_AUTO_TEST_CASE(socket__xpub_xsub__decaf__subscribed)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    std::promise<bool> received;

    // This simulates internals of a server (bs).
    simple_thread publisher_thread([&]()
    {
        zmq::socket publisher(context, role::publisher);
        BOOST_REQUIRE(publisher);
        BC_REQUIRE_SUCCESS(publisher.bind({ TEST_INPROC_ENDPOINT }));

        SEND_MESSAGES_UNTIL(publisher, received);
    });

    // This simulates a client caller (bx).
    simple_thread subscriber_thread([&]()
    {
        zmq::socket subscriber(context, role::subscriber);
        BOOST_REQUIRE(subscriber);
        BC_REQUIRE_SUCCESS(subscriber.connect({ TEST_PUBLIC_ENDPOINT }));

        RECEIVE_MESSAGE(subscriber);
        received.set_value(true);

        // The subscriber must stop so context stop will not block.
        // The context must be stopped so that zmq_proxy will return. 
        subscriber.stop();
        context.stop();
    });

    // extended_subscriber connect blocks until the endpoint is bound.
    // inproc binding connect calls also blocks until the endpoint is bound.
    // So this xsubscriber.connect must not precede spawn of publisher_thread.
    zmq::socket xsubscriber(context, role::extended_subscriber);
    BOOST_REQUIRE(xsubscriber);
    BC_REQUIRE_SUCCESS(xsubscriber.connect({ TEST_INPROC_ENDPOINT }));

    zmq::socket xpublisher(context, role::extended_publisher);
    BOOST_REQUIRE(xpublisher);
    BC_REQUIRE_SUCCESS(xpublisher.bind({ TEST_PUBLIC_ENDPOINT }));

    // The proxy returns when the current context is closed.
    // There is no difference between frontend and backend (symmetrical).
    zmq_proxy(xsubscriber.self(), xpublisher.self(), nullptr);
}

BOOST_AUTO_TEST_CASE(socket__xpub_xsub__decaf_connect_first__subscribed)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    std::promise<bool> received;

    simple_thread publisher_thread([&]()
    {
        zmq::socket publisher(context, role::publisher);
        BOOST_REQUIRE(publisher);
        BC_REQUIRE_SUCCESS(publisher.bind({ TEST_INPROC_ENDPOINT }));

        SEND_MESSAGES_UNTIL(publisher, received);
    });

    simple_thread subscriber_thread([&]()
    {
        zmq::socket subscriber(context, role::subscriber);
        BOOST_REQUIRE(subscriber);
        BC_REQUIRE_SUCCESS(subscriber.connect({ TEST_PUBLIC_ENDPOINT }));

        RECEIVE_MESSAGE(subscriber);
        received.set_value(true);
        subscriber.stop();
        context.stop();
    });

    zmq::socket xpublisher(context, role::extended_publisher);
    BOOST_REQUIRE(xpublisher);
    BC_REQUIRE_SUCCESS(xpublisher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket xsubscriber(context, role::extended_subscriber);
    BOOST_REQUIRE(xsubscriber);
    BC_REQUIRE_SUCCESS(xsubscriber.connect({ TEST_INPROC_ENDPOINT }));

    zmq_proxy(xsubscriber.self(), xpublisher.self(), nullptr);
}

// ROUTER and DEALER [request-response routing]
BOOST_AUTO_TEST_CASE(socket__router_dealer__grasslands__routed)
{
    // TODO
}

// ROUTER and DEALER [request-response routing with notifications]
BOOST_AUTO_TEST_CASE(socket__router_dealer_notifications__grasslands__notified)
{
    // TODO
}

BOOST_AUTO_TEST_SUITE_END()

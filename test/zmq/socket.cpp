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

BOOST_AUTO_TEST_CASE(socket__push_pull__grasslands_disordered__received)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    zmq::socket pusher(context, role::pusher);
    BOOST_REQUIRE(pusher);
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(context, role::puller);
    BOOST_REQUIRE(puller);
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    zmq::message out;
    out.enqueue(TEST_MESSAGE "1");
    BC_REQUIRE_SUCCESS(pusher.send(out));
    out.enqueue(TEST_MESSAGE "2");
    BC_REQUIRE_SUCCESS(pusher.send(out));

    zmq::message in;
    BC_REQUIRE_SUCCESS(puller.receive(in));
    BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE "1");
    BC_REQUIRE_SUCCESS(puller.receive(in));
    BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE "2");
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

BOOST_AUTO_TEST_CASE(socket__pair_pair__grasslands_disordered__received)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    zmq::socket server(context, role::pair);
    BOOST_REQUIRE(server);
    BC_REQUIRE_SUCCESS(server.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket client(context, role::pair);
    BOOST_REQUIRE(client);
    BC_REQUIRE_SUCCESS(client.connect({ TEST_PUBLIC_ENDPOINT }));

    zmq::message out;
    out.enqueue(TEST_MESSAGE "1");
    BC_REQUIRE_SUCCESS(client.send(out));
    out.enqueue(TEST_MESSAGE "2");
    BC_REQUIRE_SUCCESS(client.send(out));

    zmq::message in;
    BC_REQUIRE_SUCCESS(server.receive(in));
    BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE "1");
    BC_REQUIRE_SUCCESS(server.receive(in));
    BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE "2");
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

BOOST_AUTO_TEST_CASE(socket__req_rep__grasslands_disordered__bad_stream)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    zmq::socket replier(context, role::replier);
    BOOST_REQUIRE(replier);
    BC_REQUIRE_SUCCESS(replier.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket requester(context, role::requester);
    BOOST_REQUIRE(requester);
    BC_REQUIRE_SUCCESS(requester.connect({ TEST_PUBLIC_ENDPOINT }));

    zmq::message out;
    out.enqueue(TEST_MESSAGE);
    BC_REQUIRE_SUCCESS(requester.send(out));
    out.enqueue(TEST_MESSAGE);
    BOOST_REQUIRE_EQUAL(requester.send(out), error::bad_stream);
}

// REQ and ROUTER [asymetrical, synchronous, routed]
BOOST_AUTO_TEST_CASE(socket__req_router__grasslands__received)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    zmq::socket router(context, role::router);
    BOOST_REQUIRE(router);
    BC_REQUIRE_SUCCESS(router.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket requester(context, role::requester);
    BOOST_REQUIRE(requester);
    BC_REQUIRE_SUCCESS(requester.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(requester);

    zmq::message in;
    BC_REQUIRE_SUCCESS(router.receive(in));
    BOOST_REQUIRE_EQUAL(in.size(), 3u);
    BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_ROUTE_SIZE);
    BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
    BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);
}

BOOST_AUTO_TEST_CASE(socket__req_router__grasslands_connect_first__received)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    zmq::socket requester(context, role::requester);
    BOOST_REQUIRE(requester);
    BC_REQUIRE_SUCCESS(requester.connect({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket router(context, role::router);
    BOOST_REQUIRE(router);
    BC_REQUIRE_SUCCESS(router.bind({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(requester);

    zmq::message in;
    BC_REQUIRE_SUCCESS(router.receive(in));
    BOOST_REQUIRE_EQUAL(in.size(), 3u);
    BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_ROUTE_SIZE);
    BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
    BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);
}

BOOST_AUTO_TEST_CASE(socket__req_router__grasslands_disordered__bad_stream)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    zmq::socket router(context, role::router);
    BOOST_REQUIRE(router);
    BC_REQUIRE_SUCCESS(router.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket requester(context, role::requester);
    BOOST_REQUIRE(requester);
    BC_REQUIRE_SUCCESS(requester.connect({ TEST_PUBLIC_ENDPOINT }));

    zmq::message out;
    out.enqueue(TEST_MESSAGE);
    BC_REQUIRE_SUCCESS(requester.send(out));
    out.enqueue(TEST_MESSAGE);
    BOOST_REQUIRE_EQUAL(requester.send(out), error::bad_stream);
}

// REP and DEALER [asymetrical, synchronous, routed]
BOOST_AUTO_TEST_CASE(socket__req_dealer__grasslands__bad_stream)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    zmq::socket replier(context, role::replier);
    BOOST_REQUIRE(replier);
    BC_REQUIRE_SUCCESS(replier.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket dealer(context, role::dealer);
    BOOST_REQUIRE(dealer);
    BC_REQUIRE_SUCCESS(dealer.connect({ TEST_PUBLIC_ENDPOINT }));

    // The replier can only reply on an existing route.
    zmq::message out;
    out.enqueue(TEST_MESSAGE);
    BOOST_REQUIRE_EQUAL(replier.send(out), error::bad_stream);
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

// This is server v3 heartbeat service behavior.
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

    simple_thread publisher_thread([&]()
    {
        zmq::socket publisher(context, role::publisher);
        BOOST_REQUIRE(publisher);
        BC_REQUIRE_SUCCESS(publisher.bind({ TEST_INPROC_ENDPOINT }));

        SEND_MESSAGES_UNTIL(publisher, received);
        publisher.stop();
        context.stop();
    });

    simple_thread subscriber_thread([&]()
    {
        zmq::socket subscriber(context, role::subscriber);
        BOOST_REQUIRE(subscriber);
        BC_REQUIRE_SUCCESS(subscriber.connect({ TEST_PUBLIC_ENDPOINT }));

        RECEIVE_MESSAGE(subscriber);
        received.set_value(true);
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
    xsubscriber.stop();
    xpublisher.stop();
}

// This is server v3 tx/block service behavior.
// The cappucino pattern (defined here) changes the pub.bind/xsub.connect to
// pub.connect/xsub.bind, allowing the binding to remain persistent while the
// publisher comes and goes. This allows the publisher to work on various
// threads without holding connections on each of them.
BOOST_AUTO_TEST_CASE(socket__xpub_xsub__cappucino__subscribed)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    std::promise<bool> received;

    // Workers and clients come and go dynamically.

    simple_thread worker_thread([&]()
    {
        zmq::socket publisher(context, role::publisher);
        BOOST_REQUIRE(publisher);
        BC_REQUIRE_SUCCESS(publisher.connect({ TEST_INPROC_ENDPOINT }));

        SEND_MESSAGES_UNTIL(publisher, received);
        publisher.stop();
        context.stop();
    });

    simple_thread client_thread([&]()
    {
        zmq::socket subscriber(context, role::subscriber);
        BOOST_REQUIRE(subscriber);
        BC_REQUIRE_SUCCESS(subscriber.connect({ TEST_PUBLIC_ENDPOINT }));

        RECEIVE_MESSAGE(subscriber);
        received.set_value(true);
    });

    // The relay remains bound until context stop.

    zmq::socket xpublisher(context, role::extended_publisher);
    BOOST_REQUIRE(xpublisher);
    BC_REQUIRE_SUCCESS(xpublisher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket xsubscriber(context, role::extended_subscriber);
    BOOST_REQUIRE(xsubscriber);
    BC_REQUIRE_SUCCESS(xsubscriber.bind({ TEST_INPROC_ENDPOINT }));

    zmq_proxy(xsubscriber.self(), xpublisher.self(), nullptr);
    xsubscriber.stop();
    xpublisher.stop();
}

// ROUTER and DEALER [request-response routing]
BOOST_AUTO_TEST_CASE(socket__req_router_dealer_rep__synchronous_broker__replied)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    simple_thread worker_thread([&]()
    {
        zmq::socket replier(context, role::replier);
        BOOST_REQUIRE(replier);
        BC_REQUIRE_SUCCESS(replier.connect({ TEST_INPROC_ENDPOINT }));

        // Because REP is ordered the message routes with no identity.
        RECEIVE_MESSAGE(replier);
        SEND_MESSAGE(replier);
    });

    simple_thread client_thread([&]()
    {
        zmq::socket requester(context, role::requester);
        BOOST_REQUIRE(requester);
        BC_REQUIRE_SUCCESS(requester.connect({ TEST_PUBLIC_ENDPOINT }));

        // Because REQ is ordered the message routes with no identity.
        SEND_MESSAGE(requester);
        RECEIVE_MESSAGE(requester);
        requester.stop();
        context.stop();
    });

    zmq::socket router(context, role::router);
    BOOST_REQUIRE(router);
    BC_REQUIRE_SUCCESS(router.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket dealer(context, role::dealer);
    BOOST_REQUIRE(dealer);
    BC_REQUIRE_SUCCESS(dealer.bind({ TEST_INPROC_ENDPOINT }));

    zmq_proxy(router.self(), dealer.self(), nullptr);
    router.stop();
    dealer.stop();
}

// We do not use this in server even though the query worker is synchronous
// because query service also receives asynchronous responses from notifier.
BOOST_AUTO_TEST_CASE(socket__dealer_router_dealer_rep__front_asynchronous_broker__replied)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    simple_thread worker_thread([&]()
    {
        zmq::socket replier(context, role::replier);
        BOOST_REQUIRE(replier);
        BC_REQUIRE_SUCCESS(replier.connect({ TEST_INPROC_ENDPOINT }));

        RECEIVE_MESSAGE(replier);
        SEND_MESSAGE(replier);
    });

    simple_thread client_thread([&]()
    {
        zmq::socket dealer(context, role::dealer);
        BOOST_REQUIRE(dealer);
        BC_REQUIRE_SUCCESS(dealer.connect({ TEST_PUBLIC_ENDPOINT }));

        zmq::message out;
        out.enqueue();
        out.enqueue(TEST_MESSAGE);
        BC_REQUIRE_SUCCESS(dealer.send(out));

        zmq::message in;
        BC_REQUIRE_SUCCESS(dealer.receive(in));
        BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        dealer.stop();
        context.stop();
    });

    zmq::socket router(context, role::router);
    BOOST_REQUIRE(router);
    BC_REQUIRE_SUCCESS(router.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket dealer(context, role::dealer);
    BOOST_REQUIRE(dealer);
    BC_REQUIRE_SUCCESS(dealer.bind({ TEST_INPROC_ENDPOINT }));

    zmq_proxy(router.self(), dealer.self(), nullptr);
    router.stop();
    dealer.stop();
}

// This is server v3 query req/service/worker behavior.
BOOST_AUTO_TEST_CASE(socket__req_router_dealer_router__synchronous_broker__routed)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    simple_thread worker_thread([&]()
    {
        zmq::socket router(context, role::router);
        BOOST_REQUIRE(router);
        BC_REQUIRE_SUCCESS(router.connect({ TEST_INPROC_ENDPOINT }));

        // Read the enveloped request.
        // [id-dealer] from router2 for response routing (to dealer)
        // [id-req   ] from router1 for response routing (to req)
        // [delimiter] from req for data isolation
        // [data     ] from req
        zmq::message in;
        BC_REQUIRE_SUCCESS(router.receive(in));
        const auto id1 = in.dequeue_data();
        const auto id2 = in.dequeue_data();
        BOOST_REQUIRE_EQUAL(id1.size(), MESSAGE_ROUTE_SIZE);
        BOOST_REQUIRE_EQUAL(id2.size(), MESSAGE_ROUTE_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        // Reconstruct an enveloped response.
        // [id-dealer] stripped by router2
        // [id-req   ] stripped by router1
        // [delimiter] stripped by req
        // [data     ] returned by req
        zmq::message out;
        out.enqueue(id1);
        out.enqueue(id2);
        out.enqueue();
        out.enqueue(TEST_MESSAGE);
        BC_REQUIRE_SUCCESS(router.send(out));
    });

    simple_thread client_thread([&]()
    {
        zmq::socket requester(context, role::requester);
        BOOST_REQUIRE(requester);
        BC_REQUIRE_SUCCESS(requester.connect({ TEST_PUBLIC_ENDPOINT }));

        // The REQ always adds/removes delimiter (only).
        // [delimiter] from req for data isolation
        // [data     ] from req
        SEND_MESSAGE(requester);
        RECEIVE_MESSAGE(requester);
        requester.stop();
        context.stop();
    });

    zmq::socket router(context, role::router);
    BOOST_REQUIRE(router);
    BC_REQUIRE_SUCCESS(router.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket dealer(context, role::dealer);
    BOOST_REQUIRE(dealer);
    BC_REQUIRE_SUCCESS(dealer.bind({ TEST_INPROC_ENDPOINT }));

    zmq_proxy(router.self(), dealer.self(), nullptr);
    router.stop();
    dealer.stop();
}

// This is server v3 query client/service/worker behavior.
BOOST_AUTO_TEST_CASE(socket__dealer_router_dealer_router__front_asynchronous_broker__routed)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    simple_thread worker_thread([&]()
    {
        zmq::socket router(context, role::router);
        BOOST_REQUIRE(router);
        BC_REQUIRE_SUCCESS(router.connect({ TEST_INPROC_ENDPOINT }));

        // Read the enveloped request.
        // [id-dealer] from router2 for response routing (to dealer)
        // [id-req   ] from router1 for response routing (to req)
        // [delimiter] from req for data isolation
        // [data     ] passed to req
        zmq::message in;
        BC_REQUIRE_SUCCESS(router.receive(in));
        const auto id1 = in.dequeue_data();
        const auto id2 = in.dequeue_data();
        BOOST_REQUIRE_EQUAL(id1.size(), MESSAGE_ROUTE_SIZE);
        BOOST_REQUIRE_EQUAL(id2.size(), MESSAGE_ROUTE_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        // Reconstruct the enveloped response.
        // [id-dealer] stripped by router2
        // [id-req   ] stripped by router1
        // [delimiter] stripped by req
        // [data     ] returned by req
        zmq::message out;
        out.enqueue(id1);
        out.enqueue(id2);
        out.enqueue();
        out.enqueue(TEST_MESSAGE);
        BC_REQUIRE_SUCCESS(router.send(out));
    });

    simple_thread client_thread([&]()
    {
        zmq::socket dealer(context, role::dealer);
        BOOST_REQUIRE(dealer);
        BC_REQUIRE_SUCCESS(dealer.connect({ TEST_PUBLIC_ENDPOINT }));

        // Create the enveloped request (emulate REQ).
        // [delimiter] from req for data isolation
        // [data     ] from req
        zmq::message out;
        out.enqueue();
        out.enqueue(TEST_MESSAGE);
        BC_REQUIRE_SUCCESS(dealer.send(out));

        // Read the enveloped request (emulate REQ).
        // [delimiter] from req for data isolation
        // [data     ] from req
        zmq::message in;
        BC_REQUIRE_SUCCESS(dealer.receive(in));
        BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        dealer.stop();
        context.stop();
    });

    zmq::socket router(context, role::router);
    BOOST_REQUIRE(router);
    BC_REQUIRE_SUCCESS(router.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket dealer(context, role::dealer);
    BOOST_REQUIRE(dealer);
    BC_REQUIRE_SUCCESS(dealer.bind({ TEST_INPROC_ENDPOINT }));

    zmq_proxy(router.self(), dealer.self(), nullptr);
    router.stop();
    dealer.stop();
}

// This is server v2 query client/service/worker behavior (supported by v3).
// When working with dealer as opposed to router the delimiter is conventional.
// However expectation of no delimiter prevents requesters from connecting.
BOOST_AUTO_TEST_CASE(socket__dealer_router_dealer_router__front_asynchronous_broker_undelimited_request__routed)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    simple_thread worker_thread([&]()
    {
        zmq::socket router(context, role::router);
        BOOST_REQUIRE(router);
        BC_REQUIRE_SUCCESS(router.connect({ TEST_INPROC_ENDPOINT }));

        zmq::message in;
        BC_REQUIRE_SUCCESS(router.receive(in));
        const auto id1 = in.dequeue_data();
        const auto id2 = in.dequeue_data();
        BOOST_REQUIRE_EQUAL(id1.size(), MESSAGE_ROUTE_SIZE);
        BOOST_REQUIRE_EQUAL(id2.size(), MESSAGE_ROUTE_SIZE);
        ////BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        zmq::message out;
        out.enqueue(id1);
        out.enqueue(id2);
        ////out.enqueue();
        out.enqueue(TEST_MESSAGE);
        BC_REQUIRE_SUCCESS(router.send(out));
    });

    simple_thread client_thread([&]()
    {
        zmq::socket dealer(context, role::dealer);
        BOOST_REQUIRE(dealer);
        BC_REQUIRE_SUCCESS(dealer.connect({ TEST_PUBLIC_ENDPOINT }));

        zmq::message out;
        ////out.enqueue();
        out.enqueue(TEST_MESSAGE);
        BC_REQUIRE_SUCCESS(dealer.send(out));

        zmq::message in;
        BC_REQUIRE_SUCCESS(dealer.receive(in));
        ////BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        dealer.stop();
        context.stop();
    });

    zmq::socket router(context, role::router);
    BOOST_REQUIRE(router);
    BC_REQUIRE_SUCCESS(router.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket dealer(context, role::dealer);
    BOOST_REQUIRE(dealer);
    BC_REQUIRE_SUCCESS(dealer.bind({ TEST_INPROC_ENDPOINT }));

    zmq_proxy(router.self(), dealer.self(), nullptr);
    router.stop();
    dealer.stop();
}

// This is server v3 query client/service/notifier behavior.
BOOST_AUTO_TEST_CASE(socket__dealer_router_dealer_dealer__asynchronous_broker__notified)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    simple_thread worker_thread([&]()
    {
        zmq::socket dealer(context, role::dealer);
        BOOST_REQUIRE(dealer);
        BC_REQUIRE_SUCCESS(dealer.connect({ TEST_INPROC_ENDPOINT }));

        // Read the enveloped request.
        // [id-req   ] from router for response routing (to req)
        // [delimiter] from req for data isolation
        // [data     ] passed to req
        zmq::message in;
        BC_REQUIRE_SUCCESS(dealer.receive(in));
        const auto id = in.dequeue_data();
        BOOST_REQUIRE_EQUAL(id.size(), MESSAGE_ROUTE_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        // Reconstruct the enveloped response.
        // [id-req   ] stripped by router
        // [delimiter] stripped by req
        // [data     ] returned by req
        zmq::message out;
        out.enqueue(id);
        out.enqueue();
        out.enqueue(TEST_MESSAGE);
        BC_REQUIRE_SUCCESS(dealer.send(out));
    });

    simple_thread client_thread([&]()
    {
        zmq::socket dealer(context, role::dealer);
        BOOST_REQUIRE(dealer);
        BC_REQUIRE_SUCCESS(dealer.connect({ TEST_PUBLIC_ENDPOINT }));

        // Create the enveloped request (emulate REQ).
        // [delimiter] from req for data isolation
        // [data     ] from req
        zmq::message out;
        out.enqueue();
        out.enqueue(TEST_MESSAGE);
        BC_REQUIRE_SUCCESS(dealer.send(out));

        // Read the enveloped request (emulate REQ).
        // [delimiter] from req for data isolation
        // [data     ] from req
        zmq::message in;
        BC_REQUIRE_SUCCESS(dealer.receive(in));
        BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        dealer.stop();
        context.stop();
    });

    zmq::socket router(context, role::router);
    BOOST_REQUIRE(router);
    BC_REQUIRE_SUCCESS(router.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket dealer(context, role::dealer);
    BOOST_REQUIRE(dealer);
    BC_REQUIRE_SUCCESS(dealer.bind({ TEST_INPROC_ENDPOINT }));

    zmq_proxy(router.self(), dealer.self(), nullptr);
    router.stop();
    dealer.stop();
}

// The client must be a dealer for full async.
// This is server v3 query dealer/service/worker+notifier behavior.
BOOST_AUTO_TEST_CASE(socket__dealer_router_dealer_dealer_and_router__asynchronous_broker__routed_notified)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    std::promise<zmq::message> subscription;

    simple_thread response_thread([&]()
    {
        zmq::socket router(context, role::router);
        BOOST_REQUIRE(router);
        BC_REQUIRE_SUCCESS(router.connect({ TEST_INPROC_ENDPOINT }));

        zmq::message in;
        BC_REQUIRE_SUCCESS(router.receive(in));
        const auto id1 = in.dequeue_data();
        const auto id2 = in.dequeue_data();
        BOOST_REQUIRE_EQUAL(id1.size(), MESSAGE_ROUTE_SIZE);
        BOOST_REQUIRE_EQUAL(id2.size(), MESSAGE_ROUTE_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        zmq::message out;
        out.enqueue(id1);
        out.enqueue(id2);
        out.enqueue();
        out.enqueue(TEST_MESSAGE);

        // Create a notification message using the requester's route.
        // Remove router2's envelope since it will be bypassed upon return.
        auto notification = out;
        notification.dequeue();
        subscription.set_value(notification);

        // Send the response (in a race with the two notifications).
        BC_REQUIRE_SUCCESS(router.send(out));
    });

    simple_thread notification_thread([&]()
    {
        zmq::socket dealer(context, role::dealer);
        BOOST_REQUIRE(dealer);
        BC_REQUIRE_SUCCESS(dealer.connect({ TEST_INPROC_ENDPOINT }));

        // Wait on subscription and then send the notification message twice.
        auto notification1 = subscription.get_future().get();
        auto notification2 = notification1;
        BC_REQUIRE_SUCCESS(dealer.send(notification1));
        BC_REQUIRE_SUCCESS(dealer.send(notification2));
    });

    simple_thread client_thread([&]()
    {
        zmq::socket dealer(context, role::dealer);
        BOOST_REQUIRE(dealer);
        BC_REQUIRE_SUCCESS(dealer.connect({ TEST_PUBLIC_ENDPOINT }));

        // Send the initial request (simulating a subscription).
        // Create the enveloped request (emulate REQ).
        // [delimiter] from req for data isolation
        // [data     ] from req
        zmq::message out;
        out.enqueue();
        out.enqueue(TEST_MESSAGE);
        BC_REQUIRE_SUCCESS(dealer.send(out));

        // Three inbound messages, one subscription response, two notifications.
        // Read the enveloped request (emulate REQ).
        // [delimiter] from req for data isolation
        // [data     ] from req
        zmq::message in;
        BC_REQUIRE_SUCCESS(dealer.receive(in));
        BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        BC_REQUIRE_SUCCESS(dealer.receive(in));
        BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        BC_REQUIRE_SUCCESS(dealer.receive(in));
        BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        dealer.stop();
        context.stop();
    });

    zmq::socket router(context, role::router);
    BOOST_REQUIRE(router);
    BC_REQUIRE_SUCCESS(router.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket dealer(context, role::dealer);
    BOOST_REQUIRE(dealer);
    BC_REQUIRE_SUCCESS(dealer.bind({ TEST_INPROC_ENDPOINT }));

    zmq_proxy(router.self(), dealer.self(), nullptr);
    router.stop();
    dealer.stop();
}

// This is server v2 query dealer/service/worker+notifier behavior (supported by v3).
// When working with dealer as opposed to router the delimiter is conventional.
// However expectation of no delimiter prevents requesters from connecting.
BOOST_AUTO_TEST_CASE(socket__dealer_router_dealer_dealer_and_router_undelimited_request__asynchronous_broker__routed_notified)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    std::promise<zmq::message> subscription;

    simple_thread response_thread([&]()
    {
        zmq::socket router(context, role::router);
        BOOST_REQUIRE(router);
        BC_REQUIRE_SUCCESS(router.connect({ TEST_INPROC_ENDPOINT }));

        zmq::message in;
        BC_REQUIRE_SUCCESS(router.receive(in));
        const auto id1 = in.dequeue_data();
        const auto id2 = in.dequeue_data();
        BOOST_REQUIRE_EQUAL(id1.size(), MESSAGE_ROUTE_SIZE);
        BOOST_REQUIRE_EQUAL(id2.size(), MESSAGE_ROUTE_SIZE);
        ////BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        zmq::message out;
        out.enqueue(id1);
        out.enqueue(id2);
        ////out.enqueue();
        out.enqueue(TEST_MESSAGE);

        // Create a notification message using the requester's route.
        // Remove router2's envelope since it will be bypassed upon return.
        auto notification = out;
        notification.dequeue();
        subscription.set_value(notification);

        BC_REQUIRE_SUCCESS(router.send(out));
    });

    simple_thread notification_thread([&]()
    {
        zmq::socket dealer(context, role::dealer);
        BOOST_REQUIRE(dealer);
        BC_REQUIRE_SUCCESS(dealer.connect({ TEST_INPROC_ENDPOINT }));

        // Wait on subscription and then send the notification message twice.
        auto notification1 = subscription.get_future().get();
        auto notification2 = notification1;
        BC_REQUIRE_SUCCESS(dealer.send(notification1));
        BC_REQUIRE_SUCCESS(dealer.send(notification2));
    });

    simple_thread client_thread([&]()
    {
        zmq::socket dealer(context, role::dealer);
        BOOST_REQUIRE(dealer);
        BC_REQUIRE_SUCCESS(dealer.connect({ TEST_PUBLIC_ENDPOINT }));

        zmq::message out;
        ////out.enqueue();
        out.enqueue(TEST_MESSAGE);
        BC_REQUIRE_SUCCESS(dealer.send(out));

        zmq::message in;
        BC_REQUIRE_SUCCESS(dealer.receive(in));
        ////BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        BC_REQUIRE_SUCCESS(dealer.receive(in));
        ////BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        BC_REQUIRE_SUCCESS(dealer.receive(in));
        ////BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        dealer.stop();
        context.stop();
    });

    zmq::socket router(context, role::router);
    BOOST_REQUIRE(router);
    BC_REQUIRE_SUCCESS(router.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket dealer(context, role::dealer);
    BOOST_REQUIRE(dealer);
    BC_REQUIRE_SUCCESS(dealer.bind({ TEST_INPROC_ENDPOINT }));

    zmq_proxy(router.self(), dealer.self(), nullptr);
    router.stop();
    dealer.stop();
}

// This is server v3.1 query dealer/service/worker+notifier behavior.
BOOST_AUTO_TEST_CASE(socket__dealer_router_dealer_dealer_and_dealer__asynchronous_broker__routed_notified)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    std::promise<zmq::message> subscription;

    simple_thread response_thread([&]()
    {
        zmq::socket dealer(context, role::dealer);
        BOOST_REQUIRE(dealer);
        BC_REQUIRE_SUCCESS(dealer.connect({ TEST_INPROC_ENDPOINT }));

        zmq::message in;
        BC_REQUIRE_SUCCESS(dealer.receive(in));
        const auto id = in.dequeue_data();
        BOOST_REQUIRE_EQUAL(id.size(), MESSAGE_ROUTE_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        zmq::message out;
        out.enqueue(id);
        out.enqueue();
        out.enqueue(TEST_MESSAGE);
        subscription.set_value(out);
        BC_REQUIRE_SUCCESS(dealer.send(out));
    });

    simple_thread notification_thread([&]()
    {
        zmq::socket dealer(context, role::dealer);
        BOOST_REQUIRE(dealer);
        BC_REQUIRE_SUCCESS(dealer.connect({ TEST_INPROC_ENDPOINT }));

        auto notification1 = subscription.get_future().get();
        auto notification2 = notification1;
        BC_REQUIRE_SUCCESS(dealer.send(notification1));
        BC_REQUIRE_SUCCESS(dealer.send(notification2));
    });

    simple_thread client_thread([&]()
    {
        zmq::socket dealer(context, role::dealer);
        BOOST_REQUIRE(dealer);
        BC_REQUIRE_SUCCESS(dealer.connect({ TEST_PUBLIC_ENDPOINT }));

        zmq::message out;
        out.enqueue();
        out.enqueue(TEST_MESSAGE);
        BC_REQUIRE_SUCCESS(dealer.send(out));

        zmq::message in;
        BC_REQUIRE_SUCCESS(dealer.receive(in));
        BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        BC_REQUIRE_SUCCESS(dealer.receive(in));
        BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        BC_REQUIRE_SUCCESS(dealer.receive(in));
        BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        dealer.stop();
        context.stop();
    });

    zmq::socket router(context, role::router);
    BOOST_REQUIRE(router);
    BC_REQUIRE_SUCCESS(router.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket dealer(context, role::dealer);
    BOOST_REQUIRE(dealer);
    BC_REQUIRE_SUCCESS(dealer.bind({ TEST_INPROC_ENDPOINT }));

    zmq_proxy(router.self(), dealer.self(), nullptr);
    router.stop();
    dealer.stop();
}

// This is server v3.1 query dealer/service/worker+notifier (v2 compatibility).
BOOST_AUTO_TEST_CASE(socket__dealer_router_dealer_dealer_and_dealer_undelimited_request__asynchronous_broker__routed_notified)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    std::promise<zmq::message> subscription;

    simple_thread response_thread([&]()
    {
        zmq::socket dealer(context, role::dealer);
        BOOST_REQUIRE(dealer);
        BC_REQUIRE_SUCCESS(dealer.connect({ TEST_INPROC_ENDPOINT }));

        zmq::message in;
        BC_REQUIRE_SUCCESS(dealer.receive(in));
        const auto id = in.dequeue_data();
        BOOST_REQUIRE_EQUAL(id.size(), MESSAGE_ROUTE_SIZE);
        ////BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        zmq::message out;
        out.enqueue(id);
        ////out.enqueue();
        out.enqueue(TEST_MESSAGE);
        subscription.set_value(out);
        BC_REQUIRE_SUCCESS(dealer.send(out));
    });

    simple_thread notification_thread([&]()
    {
        zmq::socket dealer(context, role::dealer);
        BOOST_REQUIRE(dealer);
        BC_REQUIRE_SUCCESS(dealer.connect({ TEST_INPROC_ENDPOINT }));

        auto notification1 = subscription.get_future().get();
        auto notification2 = notification1;
        BC_REQUIRE_SUCCESS(dealer.send(notification1));
        BC_REQUIRE_SUCCESS(dealer.send(notification2));
    });

    simple_thread client_thread([&]()
    {
        zmq::socket dealer(context, role::dealer);
        BOOST_REQUIRE(dealer);
        BC_REQUIRE_SUCCESS(dealer.connect({ TEST_PUBLIC_ENDPOINT }));

        zmq::message out;
        ////out.enqueue();
        out.enqueue(TEST_MESSAGE);
        BC_REQUIRE_SUCCESS(dealer.send(out));

        zmq::message in;
        BC_REQUIRE_SUCCESS(dealer.receive(in));
        ////BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        BC_REQUIRE_SUCCESS(dealer.receive(in));
        ////BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        BC_REQUIRE_SUCCESS(dealer.receive(in));
        ////BOOST_REQUIRE_EQUAL(in.dequeue_data().size(), MESSAGE_DELIMITER_SIZE);
        BOOST_REQUIRE_EQUAL(in.dequeue_text(), TEST_MESSAGE);

        dealer.stop();
        context.stop();
    });

    zmq::socket router(context, role::router);
    BOOST_REQUIRE(router);
    BC_REQUIRE_SUCCESS(router.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket dealer(context, role::dealer);
    BOOST_REQUIRE(dealer);
    BC_REQUIRE_SUCCESS(dealer.bind({ TEST_INPROC_ENDPOINT }));

    zmq_proxy(router.self(), dealer.self(), nullptr);
    router.stop();
    dealer.stop();
}

BOOST_AUTO_TEST_SUITE_END()

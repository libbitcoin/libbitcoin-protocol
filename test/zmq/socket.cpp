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

BOOST_AUTO_TEST_CASE(socket__push_pull__shared_context__received)
{
    zmq::context context;
    BOOST_REQUIRE(context);

    zmq::socket pusher(context, role::pusher);
    BOOST_REQUIRE(pusher);
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_URL }));

    zmq::socket puller(context, role::puller);
    BOOST_REQUIRE(puller);
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_URL }));

    SEND_MESSAGE(pusher);
    WAIT_SUCCESS(puller);
    RECEIVE_MESSAGE(puller);
}

BOOST_AUTO_TEST_CASE(socket__push_pull__secure_server_secure_client__blocked)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::certificate client_certificate;
    BOOST_REQUIRE(client_certificate);

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
    BOOST_REQUIRE(puller.set_certificate(client_certificate));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_URL }));

    // There is no ZAP service running, so this fails despite configuration.
    SEND_MESSAGE(pusher);
    WAIT_FAILURE(puller);
}

BOOST_AUTO_TEST_SUITE_END()

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
#include "../utility.hpp"

using namespace bc;
using namespace bc::config;
using namespace bc::protocol;
using role = zmq::socket::role;

BOOST_AUTO_TEST_SUITE(authenticator_tests)

// context

BOOST_AUTO_TEST_CASE(authenticator__not_started__always__invalid_context)
{
    zmq::authenticator authenticator;
    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(!pusher);
}

// start

BOOST_AUTO_TEST_CASE(authenticator__started__after_socket__invalid_context)
{
    zmq::authenticator authenticator;
    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(authenticator.start());
    BOOST_REQUIRE(!pusher);
}

BOOST_AUTO_TEST_CASE(authenticator__started__before_socket__valid_context)
{
    zmq::authenticator authenticator;
    BOOST_REQUIRE(authenticator.start());
    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
}

// apply

BOOST_AUTO_TEST_CASE(authenticator__apply__public_without_server_private_key__true)
{
    zmq::authenticator authenticator;
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, false));
}

BOOST_AUTO_TEST_CASE(authenticator__apply__public_empty_domain_no_addresses__true)
{
    zmq::authenticator authenticator;
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, "", false));
}

BOOST_AUTO_TEST_CASE(authenticator__apply__public_empty_domain_with_allow__false)
{
    zmq::authenticator authenticator;
    authenticator.allow(authority{ TEST_HOST });
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(!authenticator.apply(pusher, "", false));
}

BOOST_AUTO_TEST_CASE(authenticator__apply__public_empty_domain_with_deny__false)
{
    zmq::authenticator authenticator;
    authenticator.deny(authority{ TEST_HOST });
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(!authenticator.apply(pusher, "", false));
}

BOOST_AUTO_TEST_CASE(authenticator__apply__secure_without_server_private_key__false)
{
    zmq::authenticator authenticator;
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(!authenticator.apply(pusher, TEST_DOMAIN, true));
}

BOOST_AUTO_TEST_CASE(authenticator__apply__secure_with_server_private_key__true)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::authenticator authenticator;
    authenticator.set_private_key(server_certificate.private_key());
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, true));
}

BOOST_AUTO_TEST_CASE(authenticator__apply__secure_with_empty_domain__true)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::authenticator authenticator;
    authenticator.set_private_key(server_certificate.private_key());
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, "", true));
}

// push/pull clent-server tests
// See for zeromq curve pattern: hintjens.com/blog:49

// grasslands (public and anonymous)
// The grasslands pattern does not require the authenticator.
// The authenticator is used for client indentity validation (i.e. IP address/cert/password).
// These other grasslands tests just ensure the pattern works alongside the authenticator.
// This is also useful to show that the grasslands pattern does not relay to the authenticator.

BOOST_AUTO_TEST_CASE(authenticator__push_pull__grasslands__unauthenticated__received)
{
    zmq::authenticator authenticator;
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, false));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_MESSAGE(puller);
}

BOOST_AUTO_TEST_CASE(authenticator__push_pull__grasslands_secure_certified__blocked)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::authenticator authenticator;
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, false));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    BOOST_REQUIRE(puller.set_certificate({}));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_FAILURE(puller);
}

// When misconfigured PUSH-PULL blocks on send when authenticator is REQ (vs. ROUTER).
BOOST_AUTO_TEST_CASE(authenticator__push_pull__grasslands_secure__blocked)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::authenticator authenticator;
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, false));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    ////BOOST_REQUIRE(puller.set_certificate({}));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_FAILURE(puller);
}

// When misconfigured PUSH-PULL blocks on send when authenticator is REQ (vs. ROUTER).
BOOST_AUTO_TEST_CASE(authenticator__push_pull__grasslands_certified__blocked)
{
    zmq::authenticator authenticator;
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, false));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    ////BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    BOOST_REQUIRE(puller.set_certificate({}));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_FAILURE(puller);
}

BOOST_AUTO_TEST_CASE(authenticator__push_pull__grasslands_unsecure_uncertified__received)
{
    zmq::authenticator authenticator;
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, false));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    ////BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    ////BOOST_REQUIRE(puller.set_certificate({}));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_MESSAGE(puller);
}

// strawhouse (public and anonymous with IP restrictions)

BOOST_AUTO_TEST_CASE(authenticator__push_pull__strawhouse_bad_allow__blocked)
{
    zmq::authenticator authenticator;
    authenticator.allow(authority{ TEST_HOST_BAD });
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, false));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_FAILURE(puller);
}

BOOST_AUTO_TEST_CASE(authenticator__push_pull__strawhouse_bad_and_good_allow__received)
{
    zmq::authenticator authenticator;
    authenticator.allow(authority{ TEST_HOST_BAD });
    authenticator.allow(authority{ TEST_HOST });
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, false));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_MESSAGE(puller);
}

BOOST_AUTO_TEST_CASE(authenticator__push_pull__strawhouse_client_good_allow__received)
{
    zmq::authenticator authenticator;
    authenticator.allow(authority{ TEST_HOST });
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, false));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_MESSAGE(puller);
}

BOOST_AUTO_TEST_CASE(authenticator__push_pull__strawhouse_client_bad_deny__received)
{
    zmq::authenticator authenticator;
    authenticator.deny(authority{ TEST_HOST_BAD });
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, false));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_MESSAGE(puller);
}

BOOST_AUTO_TEST_CASE(authenticator__push_pull__strawhouse_bad_and_good_deny__blocked)
{
    zmq::authenticator authenticator;
    authenticator.deny(authority{ TEST_HOST_BAD });
    authenticator.deny(authority{ TEST_HOST });
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, false));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_FAILURE(puller);
}

BOOST_AUTO_TEST_CASE(authenticator__push_pull__strawhouse_client_good_deny__blocked)
{
    zmq::authenticator authenticator;
    authenticator.deny(authority{ TEST_HOST });
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, false));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_FAILURE(puller);
}

BOOST_AUTO_TEST_CASE(authenticator__push_pull__strawhouse_good_deny_before_same_allow__blocked)
{
    zmq::authenticator authenticator;
    authenticator.deny(authority{ TEST_HOST });
    authenticator.allow(authority{ TEST_HOST });
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, false));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_FAILURE(puller);
}

BOOST_AUTO_TEST_CASE(authenticator__push_pull__strawhouse_good_deny_after_same_allow__received)
{
    zmq::authenticator authenticator;
    authenticator.allow(authority{ TEST_HOST });
    authenticator.deny(authority{ TEST_HOST });
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, false));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_MESSAGE(puller);
}

// brickhouse = stonehouse - strawhouse (private and anonymous)
// Despite client anonymity the authenitcator is required to validate the client certificate.

BOOST_AUTO_TEST_CASE(authenticator__push_pull__brickhouse_unsecure__blocked)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::authenticator authenticator;
    authenticator.set_private_key(server_certificate.private_key());
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, true));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    ////BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    BOOST_REQUIRE(puller.set_certificate({}));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_FAILURE(puller);
}

BOOST_AUTO_TEST_CASE(authenticator__push_pull__brickhouse_uncertified__blocked)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::authenticator authenticator;
    authenticator.set_private_key(server_certificate.private_key());
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, true));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    ////BOOST_REQUIRE(puller.set_certificate({}));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_FAILURE(puller);
}

// When misconfigured PUSH-PULL blocks on send when authenticator is REQ (vs. ROUTER).
BOOST_AUTO_TEST_CASE(authenticator__push_pull__brickhouse_unsecure_uncertified__blocked)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::authenticator authenticator;
    authenticator.set_private_key(server_certificate.private_key());
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, true));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_FAILURE(puller);
}

BOOST_AUTO_TEST_CASE(authenticator__push_pull__brickhouse_secure_certified__received)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::authenticator authenticator;
    authenticator.set_private_key(server_certificate.private_key());
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, true));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    BOOST_REQUIRE(puller.set_certificate({}));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_MESSAGE(puller);
}

// ironhouse (private with mututal authentication)

// When misconfigured PUSH-PULL blocks on send when authenticator is REQ (vs. ROUTER).
BOOST_AUTO_TEST_CASE(authenticator__push_pull__ironhouse_unapplied__blocked)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::certificate client_certificate;
    BOOST_REQUIRE(client_certificate);

    zmq::authenticator authenticator;
    authenticator.set_private_key(server_certificate.private_key());
    authenticator.allow(client_certificate.public_key());
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, false));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    BOOST_REQUIRE(puller.set_certificate(client_certificate));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_FAILURE(puller);
}

BOOST_AUTO_TEST_CASE(authenticator__push_pull__ironhouse_unsecure__blocked)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::certificate client_certificate;
    BOOST_REQUIRE(client_certificate);

    zmq::authenticator authenticator;
    authenticator.set_private_key(server_certificate.private_key());
    authenticator.allow(client_certificate.public_key());
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, true));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    /////BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    BOOST_REQUIRE(puller.set_certificate(client_certificate));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_FAILURE(puller);
}

BOOST_AUTO_TEST_CASE(authenticator__push_pull__ironhouse_uncertified__blocked)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::certificate client_certificate;
    BOOST_REQUIRE(client_certificate);

    zmq::authenticator authenticator;
    authenticator.set_private_key(server_certificate.private_key());
    authenticator.allow(client_certificate.public_key());
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, true));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    ////BOOST_REQUIRE(puller.set_certificate({}));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_FAILURE(puller);
}

// When misconfigured PUSH-PULL blocks on send when authenticator is REQ (vs. ROUTER).
BOOST_AUTO_TEST_CASE(authenticator__push_pull__ironhouse_unsecure_uncertified__blocked)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::certificate client_certificate;
    BOOST_REQUIRE(client_certificate);

    zmq::authenticator authenticator;
    authenticator.set_private_key(server_certificate.private_key());
    authenticator.allow(client_certificate.public_key());
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, true));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    ////BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    ////BOOST_REQUIRE(puller.set_certificate({}));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_FAILURE(puller);
}

BOOST_AUTO_TEST_CASE(authenticator__push_pull__ironhouse_unauthorized__blocked)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::certificate client_certificate;
    BOOST_REQUIRE(client_certificate);

    zmq::authenticator authenticator;
    authenticator.set_private_key(server_certificate.private_key());
    authenticator.allow(client_certificate.public_key());
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, true));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    BOOST_REQUIRE(puller.set_certificate({}));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_FAILURE(puller);
}

BOOST_AUTO_TEST_CASE(authenticator__push_pull__ironhouse_authorized__received)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::certificate client_certificate;
    BOOST_REQUIRE(client_certificate);

    zmq::authenticator authenticator;
    authenticator.set_private_key(server_certificate.private_key());
    authenticator.allow(client_certificate.public_key());
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, true));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    BOOST_REQUIRE(puller.set_certificate(client_certificate));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_MESSAGE(puller);
}

BOOST_AUTO_TEST_CASE(authenticator__push_pull__ironhouse_bad_allow__blocked)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::certificate client_certificate;
    BOOST_REQUIRE(client_certificate);

    zmq::authenticator authenticator;
    authenticator.set_private_key(server_certificate.private_key());
    authenticator.allow(client_certificate.public_key());
    authenticator.allow(authority{ TEST_HOST_BAD });
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, true));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    BOOST_REQUIRE(puller.set_certificate(client_certificate));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_FAILURE(puller);
}

// safehouse = ironhouse + strawhouse (private with mututal authentication and IP restrictions)

BOOST_AUTO_TEST_CASE(authenticator__push_pull__safehouse_bad_deny__received)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::certificate client_certificate;
    BOOST_REQUIRE(client_certificate);

    zmq::authenticator authenticator;
    authenticator.set_private_key(server_certificate.private_key());
    authenticator.allow(client_certificate.public_key());
    authenticator.deny(authority{ TEST_HOST_BAD });
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, true));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    BOOST_REQUIRE(puller.set_certificate(client_certificate));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_MESSAGE(puller);
}

BOOST_AUTO_TEST_CASE(authenticator__push_pull__safehouse_bad_and_good_deny__blocked)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::certificate client_certificate;
    BOOST_REQUIRE(client_certificate);

    zmq::authenticator authenticator;
    authenticator.set_private_key(server_certificate.private_key());
    authenticator.allow(client_certificate.public_key());
    authenticator.deny(authority{ TEST_HOST_BAD });
    authenticator.deny(authority{ TEST_HOST });
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, true));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    BOOST_REQUIRE(puller.set_certificate(client_certificate));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_FAILURE(puller);
}

BOOST_AUTO_TEST_CASE(authenticator__push_pull__safehouse_good_allow__received)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::certificate client_certificate;
    BOOST_REQUIRE(client_certificate);

    zmq::authenticator authenticator;
    authenticator.set_private_key(server_certificate.private_key());
    authenticator.allow(client_certificate.public_key());
    authenticator.allow(authority{ TEST_HOST });
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, true));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    BOOST_REQUIRE(puller.set_certificate(client_certificate));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_MESSAGE(puller);
}

BOOST_AUTO_TEST_CASE(authenticator__push_pull__safehouse_good_and_bad_allow__received)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::certificate client_certificate;
    BOOST_REQUIRE(client_certificate);

    zmq::authenticator authenticator;
    authenticator.set_private_key(server_certificate.private_key());
    authenticator.allow(client_certificate.public_key());
    authenticator.allow(authority{ TEST_HOST_BAD });
    authenticator.allow(authority{ TEST_HOST });
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, true));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    BOOST_REQUIRE(puller.set_certificate(client_certificate));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_MESSAGE(puller);
}

BOOST_AUTO_TEST_CASE(authenticator__push_pull__safehouse_good_deny__blocked)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::certificate client_certificate;
    BOOST_REQUIRE(client_certificate);

    zmq::authenticator authenticator;
    authenticator.set_private_key(server_certificate.private_key());
    authenticator.allow(client_certificate.public_key());
    authenticator.deny(authority{ TEST_HOST });
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, true));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    BOOST_REQUIRE(puller.set_certificate(client_certificate));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_FAILURE(puller);
}

BOOST_AUTO_TEST_CASE(authenticator__push_pull__safehouse_good_deny_before_same_allow__blocked)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::certificate client_certificate;
    BOOST_REQUIRE(client_certificate);

    zmq::authenticator authenticator;
    authenticator.set_private_key(server_certificate.private_key());
    authenticator.allow(client_certificate.public_key());
    authenticator.deny(authority{ TEST_HOST });
    authenticator.allow(authority{ TEST_HOST });
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, true));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    BOOST_REQUIRE(puller.set_certificate(client_certificate));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_FAILURE(puller);
}

BOOST_AUTO_TEST_CASE(authenticator__push_pull__safehouse_good_deny_after_same_allow__received)
{
    zmq::certificate server_certificate;
    BOOST_REQUIRE(server_certificate);

    zmq::certificate client_certificate;
    BOOST_REQUIRE(client_certificate);

    zmq::authenticator authenticator;
    authenticator.set_private_key(server_certificate.private_key());
    authenticator.allow(client_certificate.public_key());
    authenticator.allow(authority{ TEST_HOST });
    authenticator.deny(authority{ TEST_HOST });
    BOOST_REQUIRE(authenticator.start());

    zmq::socket pusher(authenticator, role::pusher);
    BOOST_REQUIRE(pusher);
    BOOST_REQUIRE(authenticator.apply(pusher, TEST_DOMAIN, true));
    BC_REQUIRE_SUCCESS(pusher.bind({ TEST_PUBLIC_ENDPOINT }));

    zmq::socket puller(authenticator, role::puller);
    BOOST_REQUIRE(puller);
    BOOST_REQUIRE(puller.set_curve_client(server_certificate.public_key()));
    BOOST_REQUIRE(puller.set_certificate(client_certificate));
    BC_REQUIRE_SUCCESS(puller.connect({ TEST_PUBLIC_ENDPOINT }));

    SEND_MESSAGE(pusher);
    RECEIVE_MESSAGE(puller);
}

BOOST_AUTO_TEST_SUITE_END()

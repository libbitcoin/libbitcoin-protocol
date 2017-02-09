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
#include <cassert>
#include <string>
#include <thread>
#include <bitcoin/protocol.hpp>

using namespace bc;
using namespace bc::protocol;

void server_task(const config::sodium& server_private_key,
    const config::sodium& client_public_key,
    const config::authority& client_address)
{
    // Create a threadpool for the authenticator.
    threadpool threadpool(1);

    // Establish the context's authentication whitelist.
    zmq::authenticator authenticator(threadpool);
    authenticator.allow(client_address);
    authenticator.allow(client_public_key);

    // Create a push socket using the authenticated context.
    zmq::socket server(authenticator, zmq::socket::role::pusher);
    assert(server);

    // Configure the server to provide identity and require client identity.
    auto result = server.set_private_key(server_private_key);
    assert(result);
    result = server.set_curve_server();
    assert(result);

    // Bind the server to a tcp port on all local addresses.
    auto ec = server.bind({ "tcp://*:9000" });
    assert(!ec);

    //  Send the test message.
    zmq::message message;
    message.enqueue("helllo world!");
    ec = server.send(message);
    assert(!ec);

    // Give client time to complete (normally would have external hook here).
    std::this_thread::sleep_for(asio::milliseconds(200));

    // Stop the authentication context monitor and join the thread.
    authenticator.stop();
    threadpool.join();
}

void client_task(const config::sodium& client_private_key,
    const config::sodium& server_public_key)
{
    // Create an unauthenticated context for the client.
    zmq::context context;
    assert(context);

    // Bind a pull socket to the client context.
    zmq::socket client(context, zmq::socket::role::puller);
    assert(client);

    // Configure the client to provide identity and require server identity.
    auto result = client.set_private_key(client_private_key);
    assert(result);
    result = client.set_curve_client(server_public_key);
    assert(result);

    // Connect to the server's tcp port on the local host.
    auto ec = client.connect({ "tcp://127.0.0.1:9000" });
    assert(!ec);

    // Wait for the message, which signals the test was successful.
    zmq::message message;
    ec = client.receive(message);
    assert(!ec);
    assert(message.dequeue_text() == "helllo world!");

    puts("Ironhouse test OK");
}

// TODO: move to test case.
int authenticator_example()
{
    static const auto localhost = config::authority("127.0.0.1");

    // Create client and server certificates (generated secrets).
    zmq::certificate client_cert;
    assert(client_cert);
    zmq::certificate server_cert;
    assert(server_cert);

    // Start a server, require the client cert and localhost address.
    std::thread server_thread(server_task, server_cert.private_key(),
        client_cert.public_key(), localhost);

    // Start a client, allows connections only to server with cert.
    std::thread client_thread(client_task, client_cert.private_key(),
        server_cert.public_key());

    // Wait for thread completions.
    client_thread.join();
    server_thread.join();
    return 0;
}

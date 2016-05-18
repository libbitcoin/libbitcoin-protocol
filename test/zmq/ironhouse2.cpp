/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin-protocol.
 *
 * libbitcoin-protocol is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <string>
#include <thread>
#include <boost/filesystem.hpp>
#include <bitcoin/protocol.hpp>

using namespace bc;
using namespace bc::protocol;

void client_task(const std::string& server_public_key)
{
    // Create a context.
    zmq::context context;
    assert(context);

    // Create a pull socket.
    zmq::socket client(context, zmq::socket::role::puller);
    assert(client);

    // Load our persistent certificate from disk
    zmq::certificate client_cert("client.sec");
    assert(client_cert);

    // Configure the client to provide a certificate and use the server key.
    client.set_certificate(client_cert);
    client.set_curve_client(server_public_key);

    // Connect to the server.
    const auto result = client.connect("tcp://127.0.0.1:9000");
    assert(result);

    // Wait for our message, which signals the test was successful.
    zmq::message message;
    message.receive(client);
    assert(message.parts().size() == 1);
    assert((message.parts()[0] == data_chunk{ { 0xde, 0xad, 0xbe, 0xef } }));

    puts("Ironhouse test OK");
}

void server_task(zmq::certificate& server_cert)
{
    zmq::context context;
    assert(context);

    // Start the authenticator on the context and tell it authenticate clients
    // via the certificates stored in the .curve directory.
    zmq::authenticator authenticator(context);
    authenticator.allow({ "127.0.0.1" });
    authenticator.certificates("certificates");

    // Bind a push socket to the authenticated context.
    zmq::socket server(context, zmq::socket::role::pusher);
    assert(server);
    server.set_certificate(server_cert);
    server.set_curve_server();
    const auto result = server.bind("tcp://*:9000");
    assert(result);

    //  Send our test message, just once
    zmq::message message;
    message.append({ { 0xde, 0xad, 0xbe, 0xef } });
    message.send(server);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

int ironhouse2_example()
{
    // Create the certificate store directory.
    auto result = boost::filesystem::create_directory("certificates");
    assert(result);

    // Create the client certificate.
    zmq::certificate client_cert;

    // Save the client certificate.
    result = client_cert.export_secret("client.sec");
    assert(result);

    // Save the client public certificate, for use by the server.
    result = client_cert.export_public("certificates/client.pub");
    assert(result);

    // Create the server certificate (generated values, in memory only).
    zmq::certificate server_cert;

    // Start the two detached threads, each with own ZeroMQ context.
    std::thread server_thread(server_task, std::ref(server_cert));
    std::thread client_thread(client_task, server_cert.public_key());

    client_thread.join();
    server_thread.join();
    return 0;
}

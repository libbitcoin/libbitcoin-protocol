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
#include <czmq.h>
#include <bitcoin/protocol.hpp>

using namespace bc;
using namespace bc::protocol;

////void client_task(const std::string& server_public_text)
////{
////    // Load our persistent certificate from disk
////    zmq::certificate client_cert("client_cert.txt");
////    assert(client_cert);
////
////    // Create a pull socket.
////    zmq::context context;
////    assert(context);
////    zmq::socket client(context, ZMQ_PULL);
////    assert(client);
////
////    // Configure the client to provide a certificate and use the server key.
////    client_cert.apply(client);
////    client.set_curve_serverkey(server_public_text);
////
////    // Connect to the server.
////    int rc = client.connect("tcp://127.0.0.1:9000");
////    assert(rc == 0);
////
////    // Wait for our message, which signals the test was successful.
////    zmq::message msg;
////    msg.receive(client);
////    assert(msg.parts().size() == 1);
////    assert((msg.parts()[0] == data_chunk{ { 0xde, 0xad, 0xbe, 0xef } }));
////
////    puts("Ironhouse test OK");
////}
////
////void server_task(zmq::certificate& server_cert)
////{
////    zmq::context context;
////    assert(context);
////
////    // Start the authenticator on the context and tell it authenticate clients
////    // via the certificates stored in the .curve directory.
////    zmq::authenticator authenticator(context);
////    authenticator.set_verbose(true);
////    authenticator.allow("127.0.0.1");
////    authenticator.configure_curve("*", ".curve");
////
////    // Bind a push socket to the authenticated context.
////    zmq::socket server(context, ZMQ_PUSH);
////    assert(server);
////    server_cert.apply(server);
////    server.set_curve_server();
////    int rc = server.bind("tcp://*:9000");
////    assert(rc != -1);
////
////    //  Send our test message, just once
////    zmq::message msg;
////    msg.append({ { 0xde, 0xad, 0xbe, 0xef } });
////    msg.send(server);
////
////    zclock_sleep(200);
////}
////
////int main()
////{
////    // Create the certificate store directory.
////    int rc = zsys_dir_create(".curve");
////    assert(rc == 0);
////
////    // Create the client certificate.
////    zmq::certificate client_cert;
////
////    // Save the client certificate.
////    rc = client_cert.save("client_cert.txt");
////    assert(rc == 0);
////
////    // Save the client public certificate.
////    rc = client_cert.save_public(".curve/test_cert.pub");
////    assert(rc == 0);
////
////    // Create the server certificate.
////    zmq::certificate server_cert;
////
////    // Start the two detached threads, each with own ZeroMQ context.
////    std::thread server_thread(server_task, std::ref(server_cert));
////    std::thread client_thread(client_task, server_cert.public_text());
////
////    client_thread.join();
////    server_thread.join();
////    return 0;
////}

/////**
//// * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
//// *
//// * This file is part of libbitcoin-protocol.
//// *
//// * libbitcoin-protocol is free software: you can redistribute it and/or
//// * modify it under the terms of the GNU Affero General Public License with
//// * additional permissions to the one published by the Free Software
//// * Foundation, either version 3 of the License, or (at your option)
//// * any later version. For more information see LICENSE.
//// *
//// * This program is distributed in the hope that it will be useful,
//// * but WITHOUT ANY WARRANTY; without even the implied warranty of
//// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//// * GNU Affero General Public License for more details.
//// *
//// * You should have received a copy of the GNU Affero General Public License
//// * along with this program. If not, see <http://www.gnu.org/licenses/>.
//// */
////#include <string>
////#include <thread>
////#include <czmq.h>
////#include <bitcoin/protocol.hpp>
////
////using namespace bc;
////using namespace bc::protocol;
////
////int main()
////{
////    zmq::context context;
////    assert(context.self());
////
////    // Create a few sockets
////    zmq::socket vent(context, ZMQ_PUSH);
////    int rc = vent.bind("tcp://*:9000");
////    assert(rc != -1);
////
////    zmq::socket sink(context, ZMQ_PULL);
////    rc = sink.connect("tcp://localhost:9000");
////    assert(rc != -1);
////
////    zmq::socket bowl(context, ZMQ_PULL);
////    zmq::socket dish(context, ZMQ_PULL);
////
////    // Set-up poller.
////    zmq::poller poller(bowl, sink, dish);
////    assert(poller.self());
////
////    rc = zstr_send(vent.self(), "Hello, World");
////    assert(rc != -1);
////
////    // We expect a message only on the sink.
////    auto which = poller.wait(-1);
////    assert(which == sink);
////    assert(!poller.expired());
////    assert(!poller.terminated());
////
////    auto message = zstr_recv(which.self());
////    assert(streq(message, "Hello, World"));
////
////    free(message);
////    return 0;
////}

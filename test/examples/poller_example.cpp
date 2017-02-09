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
#include <string>
#include <bitcoin/protocol.hpp>

using namespace bc;
using namespace bc::protocol;

// TODO: move to test case.
int poller_example()
{
    zmq::context context;
    assert(context);

    // Create a few sockets
    zmq::socket vent(context, zmq::socket::role::pusher);
    auto result = vent.bind({ "tcp://*:9000" });
    assert(result);

    zmq::socket sink(context, zmq::socket::role::puller);
    result = sink.connect({ "tcp://localhost:9000" });
    assert(result);

    zmq::socket bowl(context, zmq::socket::role::puller);
    zmq::socket dish(context, zmq::socket::role::puller);

    // Set-up poller.
    zmq::poller poller;
    poller.add(bowl);
    poller.add(sink);
    poller.add(dish);

    const std::string hello = "Hello, World";

    // Build and send the message.
    zmq::message message;
    message.enqueue(hello);
    auto ec = vent.send(message);
    assert(!ec);

    // We expect a message only on the sink.
    const auto ids = poller.wait(1000);
    assert(ids.contains(sink.id()));
    assert(!poller.expired());
    assert(!poller.terminated());

    // Receive the message.
    ec = sink.receive(message);
    assert(!ec);

    // Check the size.
    assert(message.size() == 1);

    // Check the value.
    const auto payload = message.dequeue_text();
    assert(payload == hello);
    return 0;
}

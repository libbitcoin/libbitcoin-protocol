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

int main_disabled()
{
    zmq::context context;
    assert(context);

    // Create a few sockets
    zmq::socket vent(context, zmq::socket::role::pusher);
    int result = vent.bind("tcp://*:9000");
    assert(result != -1);

    zmq::socket sink(context, zmq::socket::role::puller);
    result = sink.connect("tcp://localhost:9000");
    assert(result != -1);

    zmq::socket bowl(context, zmq::socket::role::puller);
    zmq::socket dish(context, zmq::socket::role::puller);

    // Set-up poller.
    zmq::poller poller;
    poller.add(bowl);
    poller.add(sink);
    poller.add(dish);

    result = zstr_send(vent.self(), "Hello, World");
    assert(result != -1);

    // We expect a message only on the sink.
    const auto id = poller.wait(-1);
    assert(id == sink.id());
    assert(!poller.expired());
    assert(!poller.terminated());

    auto message = zstr_recv(sink.self());
    assert(streq(message, "Hello, World"));

    free(message);
    return 0;
}

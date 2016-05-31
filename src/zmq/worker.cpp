/*
 * Copyright (c) 2011-2016 libbitcoin developers (see AUTHORS)
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/protocol/zmq/worker.hpp>

#include <functional>
#include <future>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/protocol/zmq/message.hpp>
#include <bitcoin/protocol/zmq/socket.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

#define NAME "worker"

// Derive from this abstract worker to implement real worker.
worker::worker(threadpool& pool)
  : dispatch_(pool, NAME),
    stopped_(true)
{
}

worker::~worker()
{
    stop();
}

// The worker is restartable but not started by default.
bool worker::start()
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    unique_lock lock(mutex_);

    if (stopped_)
    {
        stopped_ = false;
        started_ = std::promise<bool>();

        // Create the replier thread and socket and start polling.
        dispatch_.concurrent(
            std::bind(&worker::work, this));

        // Wait on replier start, convert code to bool.
        return !started_.get_future().get();
    }

    return false;
    ///////////////////////////////////////////////////////////////////////////
}

bool worker::stop()
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    unique_lock lock(mutex_);

    if (!stopped_)
    {
        stopped_ = true;
        finished_ = std::promise<bool>();

        // Wait on replier stop, convert code to bool.
        return !finished_.get_future().get();
    }

    return true;
    ///////////////////////////////////////////////////////////////////////////
}

////// Implement to define a worker type.
////void worker::work()
////{
////    // Workers can play any role.
////    socket socket(context_, socket::role::replier);
////
////    // Workers can bind or connect to the end point.
////    if (!started(socket.connect(endpoint_)))
////        return;
////
////    poller poller;
////    poller.add(socket);
////
////    // Echo messages until context termination or explicit worker stop.
////    while (!poller.terminated() && !stopped())
////        if (poller.wait().contains(socket.id()))
////            forward(socket, socket);
////
////    finished(socket.stop());
////}

// Utilities.
//-----------------------------------------------------------------------------

// Call from work to detect an explicit stop.
bool worker::stopped()
{
    return stopped_;
}

// Call from work when started (connected/bound) or failed to do so.
bool worker::started(bool result)
{
    started_.set_value(result);

    if (!result)
        finished(true);

    return result;
}

// Call from work when finished working.
bool worker::finished(bool result)
{
    started_.set_value(result);
    return result;
}

// Call from work to forward a message from one socket to another.
bool worker::forward(socket& from, socket& to)
{
    message packet;
    return packet.receive(from) && packet.send(to);
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

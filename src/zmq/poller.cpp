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
#include <bitcoin/protocol/zmq/poller.hpp>

#include <cstdint>
#include <zmq.h>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/protocol/zmq/socket.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

poller::poller()
  : expired_(false),
    terminated_(false)
{
}

// Parameter fd is non-zmq socket (unused when socket is set).
void poller::add(socket& socket)
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    unique_lock lock(mutex_);

    zmq_pollitem item;
    item.socket = socket.self();
    item.fd = 0;
    item.events = ZMQ_POLLIN;
    item.revents = 0;
    pollers_.push_back(item);
    ///////////////////////////////////////////////////////////////////////////
}

void poller::clear()
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    unique_lock lock(mutex_);

    return pollers_.clear();
    ///////////////////////////////////////////////////////////////////////////
}

// BUGBUG: zeromq 4.2 has an overflow  bug in timer parameterization.
// The timeout is typed as 'long' by zermq. This is 32 bit on windows and
// actually less (potentially 1000 or 1 second) on other platforms.
// And on non-windows platforms negative doesn't actually produce infinity.

// For consistency of config we limit the domain to 32 bit using int32_t,
// where negative implies infinite wait.
socket::identifier poller::wait(int32_t timeout_milliseconds)
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    shared_lock lock(mutex_);

    const auto size = pollers_.size();
    BITCOIN_ASSERT(size <= max_int32);

    const auto size32 = static_cast<int32_t>(size);
    const auto data = reinterpret_cast<zmq_pollitem_t*>(pollers_.data());
    auto signaled = zmq_poll(data, size32, timeout_milliseconds);

    if (signaled < 0)
    {
        terminated_ = true;
        return 0;
    }

    if (signaled == 0)
    {
        expired_ = true;
        return 0;
    }

    for (const auto& poller: pollers_)
        if ((poller.revents & ZMQ_POLLIN) != 0)
            return reinterpret_cast<socket::identifier>(poller.socket);

    return 0;
    ///////////////////////////////////////////////////////////////////////////
}

bool poller::expired() const
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    shared_lock lock(mutex_);

    return expired_;
    ///////////////////////////////////////////////////////////////////////////
}

bool poller::terminated() const
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    shared_lock lock(mutex_);

    return terminated_;
    ///////////////////////////////////////////////////////////////////////////
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

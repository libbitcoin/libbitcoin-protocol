/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/protocol/zmq/poller.hpp>

#include <cstdint>
#include <bitcoin/system.hpp>
#include <bitcoin/protocol/zmq/identifiers.hpp>
#include <bitcoin/protocol/zmq/socket.hpp>
#include <bitcoin/protocol/zmq/zeromq.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

using namespace bc::system;

poller::poller() NOEXCEPT
  : expired_(false),
    terminated_(false)
{
}

// Parameter fd is non-zmq socket (unused when socket is set).
void poller::add(socket& socket) NOEXCEPT
{
    zmq_pollitem item;
    item.socket = socket.self();
    item.fd = 0;
    item.events = ZMQ_POLLIN;
    item.revents = 0;
    pollers_.push_back(item);
}

void poller::clear() NOEXCEPT
{
    return pollers_.clear();
}

identifiers poller::wait() NOEXCEPT
{
    return wait(zmq_maximum_safe_wait_milliseconds);
}

// BUGBUG: zeromq 4.2 has an overflow bug in timer parameterization.
// The timeout is typed as 'long' by zeromq. This is 32 bit on windows and
// actually less (potentially 1000 or 1 second) on other platforms.
// On non-windows platforms negative doesn't actually produce infinity.
identifiers poller::wait(int32_t timeout_milliseconds) NOEXCEPT
{
    const auto size = pollers_.size();
    BC_ASSERT(size <= max_int32);

    const auto count = possible_narrow_sign_cast<int32_t>(size);
    const auto& items = pointer_cast<zmq_pollitem_t>(pollers_.data());
    const auto signaled = zmq_poll(items, count, timeout_milliseconds);

    // Either one of the sockets was terminated or a signal intervened.
    if (is_negative(signaled))
    {
        terminated_ = true;
        return {};
    }

    // No events have been signaled and no failure, so the timer expired.
    if (is_zero(signaled))
    {
        expired_ = true;
        return {};
    }

    // At least one event was signaled, but the poll-in set may be empty.
    identifiers result;
    for (const auto& poller: pollers_)
        if (!is_zero(poller.revents & ZMQ_POLLIN))
            result.push(poller.socket);

    return result;
}

bool poller::expired() const NOEXCEPT
{
    return expired_;
}

bool poller::terminated() const NOEXCEPT
{
    return terminated_;
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

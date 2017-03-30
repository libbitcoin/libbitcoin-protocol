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
#ifndef LIBBITCOIN_PROTOCOL_ZMQ_POLLER_HPP
#define LIBBITCOIN_PROTOCOL_ZMQ_POLLER_HPP

#include <algorithm>
#include <cstdint>
#include <memory>
#include <vector>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/zmq/identifiers.hpp>
#include <bitcoin/protocol/zmq/socket.hpp>
#include <bitcoin/protocol/zmq/zeromq.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

/// This class is not thread safe.
/// All calls must be made on the socket(s) thread.
class BCP_API poller
  : public enable_shared_from_base<poller>, noncopyable
{
public:
    /// A shared poller pointer.
    typedef std::shared_ptr<poller> ptr;

    /// Construct an empty poller (sockets must be added).
    poller();

    /// True if the timeout occurred.
    bool expired() const;

    /// True if the connection is closed.
    bool terminated() const;

    /// Add a socket to be polled.
    void add(socket& sock);

    /// Remove all sockets from the poller.
    void clear();

    /// Wait one second for any socket to receive.
    identifiers wait();

    /// Wait specified time for any socket to receive, -1 is forever.
    identifiers wait(int32_t timeout_milliseconds);

private:
    typedef std::vector<zmq_pollitem> pollers;

    // These values are unprotected.
    bool expired_;
    bool terminated_;
    pollers pollers_;
};

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif

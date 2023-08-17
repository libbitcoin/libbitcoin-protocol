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
#ifndef LIBBITCOIN_PROTOCOL_ZMQ_CONTEXT_HPP
#define LIBBITCOIN_PROTOCOL_ZMQ_CONTEXT_HPP

#include <atomic>
#include <cstdint>
#include <memory>
#include <shared_mutex>
#include <bitcoin/system.hpp>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/network.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

/// This class is thread safe.
class BCP_API context
  : public enable_shared_from_base<context>
{
public:
    DELETE_COPY_MOVE(context);

    /// A shared context pointer.
    typedef std::shared_ptr<context> ptr;

    /// Construct a context.
    context(bool started=true) NOEXCEPT;

    /// Blocks until all child sockets are closed.
    /// Stops all child socket activity by closing the zeromq context.
    ~context() NOEXCEPT;

    /// True if the context is valid and started.
    operator bool() const NOEXCEPT;

    /// The underlying zeromq context.
    void* self() NOEXCEPT;

    /// Create the zeromq context.
    bool start() NOEXCEPT;

    /// Blocks until all child sockets are closed.
    /// Stops all child socket activity by closing the zeromq context.
    bool stop() NOEXCEPT;

private:
    // This is thread safe
    std::atomic<void*> self_;

    // This guards against a start/stop race.
    mutable std::shared_mutex mutex_;
};

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif


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
#include <bitcoin/protocol/zmq/context.hpp>

#include <cstdint>
#include <zmq.h>
#include <bitcoin/bitcoin.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

static constexpr int32_t zmq_fail = -1;

context::context(bool started)
  : self_(nullptr)
{
    if (started)
        start();
}

context::~context()
{
    stop();
}

// Restartable after stop and optionally started on construct.
bool context::start()
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    unique_lock lock(mutex_);

    if (self_ != nullptr)
        return false;

    self_.store(zmq_ctx_new());
    return self_ != nullptr;
    ///////////////////////////////////////////////////////////////////////////
}

// Signal termination and block until all sockets closed.
bool context::stop()
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    unique_lock lock(mutex_);

    if (self_ == nullptr)
        return true;

    // This aborts blocking operations but blocks here until either each socket
    // in the context is explicitly closed. This can fail by signal interrupt.
    const auto result = zmq_ctx_term(self_) != zmq_fail;

    self_.store(nullptr);
    return result;
    ///////////////////////////////////////////////////////////////////////////
}

context::operator bool() const
{
    return self_ != nullptr;
}

// This may become invalid after return. This call only ensures atomicity.
void* context::self()
{
    return self_;
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

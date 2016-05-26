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
#include <bitcoin/protocol/zmq/context.hpp>

#include <cstdint>
#include <zmq.h>
#include <bitcoin/bitcoin.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

static constexpr int32_t zmq_fail = -1;

context::context()
  : self_(zmq_ctx_new())
{
}

context::~context()
{
    stop();
}

// This could be made non-blocking by using zmq_ctx_shutdown here and
// zmq_ctx_term in a close method (invoked from the destructor).
bool context::stop()
{
    if (self() == nullptr)
        return true;

    // This aborts blocking operations but blocks here until either each socket
    // in the context is explicitly closed.
    // It is possible for this to fail due to signal interrupt.
    const auto result = zmq_ctx_term(self_) != zmq_fail;
    self_ = nullptr;
    return result;
}

context::operator const bool() const
{
    return self_ != nullptr;
}

void* context::self()
{
    return self_;
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

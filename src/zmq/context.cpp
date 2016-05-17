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
static constexpr int32_t zmq_io_threads = 1;

// TODO: Each socket should maintain a smart pointer reference to the context.
// TODO: When all sockets are closed the context is free to be destroyed.
// TODO: Add call to zmq_term on the context that causes all sockets to close.
context::context()
  : threads_(zmq_io_threads),
    self_(zmq_init(threads_))
{
}

context::~context()
{
    DEBUG_ONLY(const auto result =) destroy();
    BITCOIN_ASSERT(result);
}

bool context::destroy()
{
    if (self_ == nullptr)
        return true;

    // This will cause all related sockets to close and will block until
    // all sockets open within context have been closed with zmq_close().
    return zmq_term(self_) != zmq_fail;
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

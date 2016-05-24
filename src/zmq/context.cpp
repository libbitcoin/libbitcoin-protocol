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

context::context()
  : threads_(zmq_io_threads),
    self_(zmq_init(threads_))
{
}

context::~context()
{
    DEBUG_ONLY(const auto result =) close();
    BITCOIN_ASSERT(result);
}

bool context::close()
{
    if (self_ == nullptr)
        return true;

    // This aborts blocking operations but blocks here until either each socket
    // in the context is explicitly closed or its linger period is exceeded.
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

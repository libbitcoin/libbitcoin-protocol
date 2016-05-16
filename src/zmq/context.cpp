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

#include <czmq.h>
#include <bitcoin/bitcoin.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

// TODO: the member self_ will be a void*.
// TODO: Each socket should maintain a smart pointer reference to the context.
// TODO: When all sockets are closed the context is free to be destroyed.
context::context()
  : self_(zctx_new() /* zmq_init(self->iothreads) */)
{
    // TODO: configure iothreads, default 1 in zmq/zctx_new.
    ////self_ = zmq_init(self->iothreads);
}

context::~context()
{
    BITCOIN_ASSERT(self_);

    //// This will cause all related sockets to terminate.
    ////zmq_term(self_);

    zctx_destroy(&self_);
}

context::operator const bool() const
{
    return self_ != nullptr;
}

zctx_t* context::self()
{
    return self_;
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

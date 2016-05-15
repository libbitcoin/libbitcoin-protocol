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

context::context()
  : self_(zctx_new())
{
    // Disable czmq signal handling.
    zsys_handler_set(NULL);

#ifdef _MSC_VER
    // Hack to prevent czmq from writing to stdout/stderr on Windows.
    // This will prevent authentication feedback, but also prevent crashes.
    // It is necessary to prevent stdio when using our utf8-everywhere pattern.
    // TODO: provide a FILE* here that we can direct to our own log/console.
    zsys_set_logstream(NULL);
#endif
}

context::~context()
{
    BITCOIN_ASSERT(self_);
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

/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/protocol/zmq/zeromq.hpp>

#include <zmq.h>
#include <bitcoin/system.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

// See zmq::errno_to_string
system::code get_last_error()
{
    const auto zeromq_code = zmq_errno();
    const auto ec = system::error::posix_to_error_code(zeromq_code);

    if (ec != system::error::unknown)
        return ec;

    switch (zeromq_code)
    {
        // See zmq.h
        case EFSM:
            return system::error::bad_stream;
        case ETERM:
            return system::error::service_stopped;
        case EMTHREAD:
        case ENOCOMPATPROTO:
            return system::error::operation_failed;
        default:
            return system::error::unknown;
    }
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

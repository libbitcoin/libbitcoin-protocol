/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/protocol/zmq/error.hpp>

#include <zmq.h>
#include <bitcoin/system.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {
namespace error {

DEFINE_ERROR_T_MESSAGE_MAP(error)
{
    { success, "success" },
    { unknown, "unknown error" },

    { socket_state, "socket not in appropriate state" },
    { context_terminated, "socket context was terminated" },
    { no_thread, "no thread available" },
    { incompatible_protocol, "protocol incompatible with socket type" },

    { host_unreachable, "host unreachable" },
    { no_buffer_space, "no buffer space available" },
    { unsupported_operation, "operation not supported" },
    { unsupported_protocol, "protocol not supported" },
    { network_down, "network down" },
    { address_in_use, "address in use" },
    { resolve_failed, "address resolution failed" },
    { accept_failed, "connection refused" },
    { in_progress, "operation in progress" },
    { try_again, "non-blocking request but message cannot be sent now" },
    { invalid_message, "invalid message" },
    { interrupted, "operation interrupted by signal before send" },
    { invalid_socket, "invalid socket" }
};

DEFINE_ERROR_T_CATEGORY(error, "protocol", "protocol code")

// These are only error codes defined by zeromq or undefined on some Windows.
// Any posix code may also be returned, which will translate to "unknown"
// unless it is added here.
code get_last_error() NOEXCEPT
{
    // api.zeromq.org/3-2:zmq-errno
    switch (zmq_errno())
    {
        case 0:
            return error::success;

        // zeromq-defined posix-style codes (defined in zmq.h)
        case EFSM:
            return error::socket_state;
        case ETERM:
            return error::context_terminated;
        case EMTHREAD:
            return error::no_thread;
        case ENOCOMPATPROTO:
            return error::incompatible_protocol;

        // posix codes (used by zeromq but not be defined in some Windows)
        case EHOSTUNREACH:
            return error::host_unreachable;
        case ENOBUFS:
            return error::no_buffer_space;
        case ENOTSUP:
            return error::unsupported_operation;
        case EPROTONOSUPPORT:
            return error::unsupported_protocol;
        case ENETDOWN:
            return error::network_down;
        case EADDRINUSE:
            return error::address_in_use;
        case EADDRNOTAVAIL:
            return error::resolve_failed;
        case ECONNREFUSED:
            return error::accept_failed;
        case EINPROGRESS:
            return error::in_progress;
        case EAGAIN:
            return error::try_again;
        case EFAULT:
            return error::invalid_message;
        case EINTR:
            return error::interrupted;
        case ENOTSOCK:
            return error::invalid_socket;

        default:
            return error::unknown;
    }
}

} // namespace error
} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

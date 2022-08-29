/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_PROTOCOL_ERROR_HPP
#define LIBBITCOIN_PROTOCOL_ERROR_HPP

#include <system_error>
#include <bitcoin/system.hpp>
#include <bitcoin/protocol/define.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {
namespace error {

/// Alias system code.
using code = std::error_code;

enum error_t
{
    success = 0,
    unknown,
    socket_state,
    context_terminated,
    no_thread,
    incompatible_protocol,
    host_unreachable,
    no_buffer_space,
    unsupported_operation,
    unsupported_protocol,
    network_down,
    address_in_use,
    resolve_failed,
    accept_failed,
    in_progress,
    try_again,
    invalid_message,
    interrupted,
    invalid_socket
};

// No current need for error_code equivalence mapping.
DECLARE_ERROR_T_CODE_CATEGORY(error);

/// Obtain the last error for this thread.
code BCP_API get_last_error() NOEXCEPT;

} // namespace error
} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

DECLARE_STD_ERROR_REGISTRATION(bc::protocol::zmq::error::error)

#endif

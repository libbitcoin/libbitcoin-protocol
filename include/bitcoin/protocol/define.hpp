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
#ifndef LIBBITCOIN_PROTOCOL_DEFINE_HPP
#define LIBBITCOIN_PROTOCOL_DEFINE_HPP

#include <memory>
#include <queue>
#include <bitcoin/system.hpp>

// We use the generic helper definitions in libbitcoin to define BCP_API
// and BCP_INTERNAL. BCP_API is used for the public API symbols. It either DLL
// imports or DLL exports (or does nothing for static build) BCP_INTERNAL is
// used for non-api symbols.

#if defined BCP_STATIC
    #define BCP_API
    #define BCP_INTERNAL
#elif defined BCP_DLL
    #define BCP_API      BC_HELPER_DLL_EXPORT
    #define BCP_INTERNAL BC_HELPER_DLL_LOCAL
#else
    #define BCP_API      BC_HELPER_DLL_IMPORT
    #define BCP_INTERNAL BC_HELPER_DLL_LOCAL
#endif

// Log name.
#define LOG_PROTOCOL "protocol"
#define LOG_PROTOCOL_HTTP "http"

// TODO: generalize logging.
#define LOG_INFO(name) std::cout << name << " : "
#define LOG_DEBUG(name) std::cout << name << " : "
#define LOG_VERBOSE(name) std::cout << name << " : "
#define LOG_ERROR(name) std::cerr << name << " : "
#define LOG_WARNING(name) std::cerr << name << " : "

namespace libbitcoin {
namespace protocol {

typedef std::queue<system::data_chunk> data_queue;

} // namespace protocol
} // namespace libbitcoin

#endif

// context       ->
// sodium        ->
// identifiers   ->
// worker        -> socket
// message       -> socket
// certificate   -> sodium
// socket        -> sodium, context, certificate, identifiers
// authenticator -> sodium, context, socket, worker
// poller        -> socket, zeromq
// frame         -> socket, zeromq

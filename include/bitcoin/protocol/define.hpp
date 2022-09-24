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

#include <queue>
#include <bitcoin/system.hpp>

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

#define LOG_INFO(name) std::cout << name << " : "
#define LOG_DEBUG(name) std::cout << name << " : "
#define LOG_VERBOSE(name) std::cout << name << " : "
#define LOG_ERROR(name) std::cerr << name << " : "
#define LOG_WARNING(name) std::cerr << name << " : "

#if defined _WIN32
    #include <winsock2.h>
    typedef SOCKET file_descriptor;
#else
    typedef int file_descriptor;
#endif

// TODO: also in network.
#ifdef _MSC_VER
    #include <windows.h>
#else
    #include <unistd.h>
    #include <pthread.h>
    #include <sys/resource.h>
    #include <sys/types.h>
    #ifndef PRIO_MAX
        #define PRIO_MAX 20
    #endif
    #define THREAD_PRIORITY_ABOVE_NORMAL (-2)
    #define THREAD_PRIORITY_NORMAL 0
    #define THREAD_PRIORITY_BELOW_NORMAL 2
    #define THREAD_PRIORITY_LOWEST PRIO_MAX
#endif

#include <memory>

namespace libbitcoin {
namespace protocol {

// TODO: also in network.
template <class Base>
class enable_shared_from_base
    : public std::enable_shared_from_this<Base>
{
public:
    ~enable_shared_from_base() noexcept
    {
    }

protected:
    template <class Derived, bc::if_base_of<Base, Derived> = true>
    std::shared_ptr<Derived> shared_from_base() noexcept
    {
        return std::static_pointer_cast<Derived>(this->shared_from_this());
    }
};

// TODO: implement, also in network.
class noncopyable {};

typedef std::queue<system::data_chunk> data_queue;

// TODO: also in network.
enum class thread_priority
{
    high,
    normal,
    low,
    lowest
};

// TODO: also in network.
// Privately map the class enum thread priority value to an integer.
inline int get_priority(thread_priority priority) noexcept
{
    switch (priority)
    {
        case thread_priority::lowest:
            return THREAD_PRIORITY_LOWEST;
        case thread_priority::low:
            return THREAD_PRIORITY_BELOW_NORMAL;
        case thread_priority::high:
            return THREAD_PRIORITY_ABOVE_NORMAL;
        default:
        case thread_priority::normal:
            return THREAD_PRIORITY_NORMAL;
    }
}

// TODO: also in network.
// Set the thread priority (or process if thread priority is not available).
inline void set_priority(thread_priority priority) noexcept
{
    const auto prioritization = get_priority(priority);

#if defined(_MSC_VER)
    SetThreadPriority(GetCurrentThread(), prioritization);
#elif defined(PRIO_THREAD)
    setpriority(PRIO_THREAD, pthread_self(), prioritization);
#else
    setpriority(PRIO_PROCESS, getpid(), prioritization);
#endif
}

} // namespace protocol
} // namespace libbitcoin

#endif

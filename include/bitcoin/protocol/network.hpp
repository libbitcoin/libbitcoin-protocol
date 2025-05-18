/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_PROTOCOL_NETWORK_HPP
#define LIBBITCOIN_PROTOCOL_NETWORK_HPP

// These are also defined in libbitcoin-network.
// Defined here to avoid the network dependency.
// config::authority and config::endpoint are also cloned from network.

#include <memory>
#include <thread>
#include <bitcoin/system.hpp>

#ifdef HAVE_MSC
    #include <windows.h>
    // #define THREAD_PRIORITY_IDLE            -15
    // #define THREAD_PRIORITY_LOWEST          -2
    // #define THREAD_PRIORITY_BELOW_NORMAL    -1
    // #define THREAD_PRIORITY_NORMAL           0
    // #define THREAD_PRIORITY_ABOVE_NORMAL     1
    // #define THREAD_PRIORITY_HIGHEST          2
    // #define THREAD_PRIORITY_TIME_CRITICAL    15
    // #define THREAD_PRIORITY_ERROR_RETURN    (MAXLONG)
#else
    #include <unistd.h>
    #include <pthread.h>
    #include <sys/resource.h>
    #include <sys/types.h>
    #define THREAD_PRIORITY_HIGHEST             -20
    #define THREAD_PRIORITY_ABOVE_NORMAL        -2
    #define THREAD_PRIORITY_NORMAL               0
    #define THREAD_PRIORITY_BELOW_NORMAL         2
    #define THREAD_PRIORITY_LOWEST               20
#endif

namespace libbitcoin {
namespace protocol {

typedef system::data_array<16> ip_address;

// Empty base optimization using CRTP.
template <class Base>
class enable_shared_from_base
  : public std::enable_shared_from_this<Base>
{
protected:
    template <class Derived, bc::if_base_of<Base, Derived> = true>
    std::shared_ptr<Derived> shared_from_base() NOEXCEPT
    {
        return std::static_pointer_cast<Derived>(this->shared_from_this());
    }
};

enum class thread_priority
{
    highest,
    high,
    normal,
    low,
    lowest
};

// Privately map the class enum thread priority value to an integer.
inline int get_priority(thread_priority priority) NOEXCEPT
{
    switch (priority)
    {
        case thread_priority::lowest:
            return THREAD_PRIORITY_LOWEST;
        case thread_priority::low:
            return THREAD_PRIORITY_BELOW_NORMAL;
        case thread_priority::high:
            return THREAD_PRIORITY_ABOVE_NORMAL;
        case thread_priority::highest:
            return THREAD_PRIORITY_HIGHEST;
        default:
        case thread_priority::normal:
            return THREAD_PRIORITY_NORMAL;
    }
}

// Set the thread priority.
// TODO: handle error conditions.
// TODO: handle potential lack of PRIO_THREAD
// TODO: use proper non-win32 priority levels.
// TODO: Linux: pthread_setschedprio()
// TOOD: macOS: somethign else.
inline void set_priority(thread_priority priority) NOEXCEPT
{
    const auto prioritization = get_priority(priority);

#if defined(HAVE_MSC)
    // learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/
    // nf-processthreadsapi-getthreadpriority
    SetThreadPriority(GetCurrentThread(), prioritization);

#elif defined(PRIO_THREAD)
    // lore.kernel.org/lkml/1220278355.3866.21.camel@localhost.localdomain/
    setpriority(PRIO_THREAD, pthread_self(), prioritization);

#else
    // BUGBUG: This will set all threads in the process.
    // man7.org/linux/man-pages/man3/pthread_self.3.html
    setpriority(PRIO_PROCESS, getpid(), prioritization);
#endif
}

inline size_t cores() NOEXCEPT
{
    return std::max(std::thread::hardware_concurrency(), 1_u32);
}

} // namespace protocol
} // namespace libbitcoin

#endif

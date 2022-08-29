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
#ifndef LIBBITCOIN_PROTOCOL_NETWORK_HPP
#define LIBBITCOIN_PROTOCOL_NETWORK_HPP

// These are also defined in libbitcoin-network.
// Defined here to avoid the network dependency.
// config::authority and config::endpoint are also cloned from network.

#include <memory>
#include <bitcoin/system.hpp>

#ifdef HAVE_MSC
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

namespace libbitcoin {
namespace protocol {

typedef system::data_array<16> ip_address;

// Empty base optimization using CRTP.
template <class Type>
class noncopyable
{
public:
    noncopyable(const noncopyable&) = delete;
    Type& operator = (const Type&) = delete;

protected:
    noncopyable() = default;
    
    // non-virtual
    ~noncopyable() = default;
};

// Empty base optimization using CRTP.
template <class Base>
class enable_shared_from_base
  : public std::enable_shared_from_this<Base>
{
public:
    // non-virtual
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

enum class thread_priority
{
    high,
    normal,
    low,
    lowest
};

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

// Set the thread priority (or process if thread priority is not available).
inline void set_priority(thread_priority priority) noexcept
{
    const auto prioritization = get_priority(priority);

#if defined(HAVE_MSC)
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

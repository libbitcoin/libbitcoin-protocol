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
#ifndef LIBBITCOIN_PROTOCOL_ZMQ_ZEROMQ_HPP
#define LIBBITCOIN_PROTOCOL_ZMQ_ZEROMQ_HPP

#include <cstddef>
#include <cstdint>
#if defined HAVE_MSC
    #include <winsock2.h>
#endif
#include <zmq.h>
#include <bitcoin/system.hpp>
#include <bitcoin/protocol/define.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

/// Define symbols not defined by zeromq.
constexpr auto zmq_subscribe_all = "";
constexpr int32_t zmq_true = 1;
constexpr int32_t zmq_false = 0;
constexpr int32_t zmq_fail = -1;
constexpr int32_t zmq_reconnect_interval = 100;
constexpr size_t zmq_encoded_key_size = 40;

// This is the maximum safe value on all platforms, due to zeromq bug.
constexpr int32_t zmq_maximum_safe_wait_milliseconds = 1000;

// If ZMQ_DONTWAIT is set we fail on busy socket.
// This would happen if a message is being read when we try to send.
constexpr int32_t wait_flag = 0;

/// zmq_msg_t alias, keeps zmq.h out of our headers.
/// Conditions are based on zeromq declarations.
typedef struct zmq_msg
{
#if defined(HAVE_GNUC) || defined(__INTEL_COMPILER) || \
    (defined(__SUNPRO_C) && __SUNPRO_C >= 0x590) || \
    (defined(__SUNPRO_CC) && __SUNPRO_CC >= 0x590)
    unsigned char _[64] __attribute__ ((aligned(sizeof(void*))));
#elif defined(HAVE_MSC) && (defined(_M_X64) || defined(_M_ARM64))
    __declspec (align(8)) unsigned char _[64];
#elif defined(HAVE_MSC) && (defined(_M_IX86) || defined(_M_ARM_ARMV7VE))
    __declspec (align(4)) unsigned char _[64];
#else
    unsigned char _[64];
#endif
} zmq_msg;

#if defined(HAVE_MSC)
    typedef SOCKET file_descriptor;
#else
    typedef int file_descriptor;
#endif

/// zmq_pollitem_t alias, keeps zmq.h out of our headers.
typedef struct zmq_pollitem
{
    void* socket;
    file_descriptor fd;
    short events;
    short revents;
} zmq_pollitem;

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif

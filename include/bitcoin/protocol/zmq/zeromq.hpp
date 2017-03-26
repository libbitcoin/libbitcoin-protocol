/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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

#include <bitcoin/bitcoin.hpp>
#include <bitcoin/protocol/define.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

// zmq_msg_t alias, keeps zmq.h out of our headers.
typedef struct zmq_msg
{
#if defined(__GNUC__) || defined(__INTEL_COMPILER) || \
    (defined(__SUNPRO_C) && __SUNPRO_C >= 0x590) || \
    (defined(__SUNPRO_CC) && __SUNPRO_CC >= 0x590)
    unsigned char _[64] __attribute__ ((aligned(sizeof(void*))));
#elif defined(_MSC_VER) && (defined(_M_X64) || defined(_M_ARM64))
    __declspec (align(8)) unsigned char _[64];
#elif defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_ARM_ARMV7VE))
    __declspec (align(4)) unsigned char _[64];
#else
    unsigned char _[64];
#endif
} zmq_msg;

// zmq_pollitem_t alias, keeps zmq.h out of our headers.
typedef struct zmq_pollitem
{
    void* socket;
#if defined(_WIN32)
    SOCKET fd;
#else
    int fd;
#endif
    short events;
    short revents;
} zmq_pollitem;

code BCP_API get_last_error();

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif

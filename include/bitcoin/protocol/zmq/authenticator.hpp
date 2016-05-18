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
#ifndef LIBBITCOIN_PROTOCOL_ZMQ_AUTHENTICATOR_HPP
#define LIBBITCOIN_PROTOCOL_ZMQ_AUTHENTICATOR_HPP

#include <string>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/zmq/context.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

class BCP_API authenticator
{
public:
    /// Construct an instance.
    authenticator(context& context);

    /// Free authenticator resources.
    ~authenticator();

    /// This class is not copyable.
    authenticator(const authenticator&) = delete;
    void operator=(const authenticator&) = delete;

    /// True if the construction succeeded.
    operator const bool() const;

    /// Allow clients with the following public keys (white list).
    void allow(const std::string& public_key);

    /// Allow clients with the following ip addresses (white list).
    void allow(const config::authority& address);

    /// Allow clients with the following ip addresses (black list).
    void deny(const config::authority& address);

private:
    void* authenticator_;
};

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif


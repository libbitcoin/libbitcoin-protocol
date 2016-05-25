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

#include <memory>
#include <string>
#include <unordered_map>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/zmq/context.hpp>
#include <bitcoin/protocol/zmq/poller.hpp>
#include <bitcoin/protocol/zmq/socket.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

/// This class is not thread safe.
class BCP_API authenticator
  : public context
{
public:
    /// A shared authenticator pointer.
    typedef std::shared_ptr<authenticator> ptr;

    /// Start the ZAP monitor for the context.
    /// The threadpool must be joined prior to destruct.
    /// There may be only one authenticator per process (otherwise bridge).
    authenticator(threadpool& threadpool);

    /// Start the ZAP monitor.
    bool start();

    /// Allow clients with the following public keys (whitelist).
    void allow(const hash_digest& public_key);

    /// Allow clients with the following ip addresses (whitelist).
    void allow(const config::authority& address);

    /// Allow clients with the following ip addresses (blacklist).
    void deny(const config::authority& address);

private:
    void monitor();
    bool allowed(const std::string& address) const;
    bool allowed(const hash_digest& public_key) const;

    // These are not thread safe, they are protected by sequential access.
    socket socket_;
    poller poller_;
    dispatcher dispatch_;
    bool require_address_;
    std::unordered_map<hash_digest, bool> keys_;
    std::unordered_map<std::string, bool> adresses_;
};

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif


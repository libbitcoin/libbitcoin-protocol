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
#ifndef LIBBITCOIN_PROTOCOL_ZMQ_AUTHENTICATOR_HPP
#define LIBBITCOIN_PROTOCOL_ZMQ_AUTHENTICATOR_HPP

#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <bitcoin/system.hpp>
#include <bitcoin/protocol/config/config.hpp>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/zmq/context.hpp>
#include <bitcoin/protocol/zmq/socket.hpp>
#include <bitcoin/protocol/zmq/worker.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

/// This class is thread safe.
class BCP_API authenticator
  : public worker
{
public:
    DELETE_COPY_MOVE(authenticator);

    /// A shared authenticator pointer.
    typedef std::shared_ptr<authenticator> ptr;

    /// The fixed inprocess authentication endpoint.
    static const endpoint authentication_point;

    /// There may be only one authenticator per process.
    authenticator(thread_priority priority=thread_priority::normal) NOEXCEPT;

    /// Stop the router.
    virtual ~authenticator() NOEXCEPT;

    /// Expose the authenticated context.
    operator context&() NOEXCEPT;

    /// Start the ZAP router for the context.
    virtual bool start() NOEXCEPT override;

    /// Stop the router (optional).
    virtual bool stop() NOEXCEPT override;

    /// This must be called on the socket thread.
    /// A nonempty domain is required for the NULL mechanism.
    /// Set secure false to enable NULL mechanism, otherwise curve is required.
    /// Apply authentication to the socket for the given arbitrary domain.
    /// Behavior is based on set_private_key and allow/deny configuration.
    virtual bool apply(socket& socket, const std::string& domain,
        bool secure) NOEXCEPT;

    /// Set the server private key (required for curve security).
    virtual void set_private_key(const sodium& private_key) NOEXCEPT;

    /// Allow clients with the following public keys (whitelist).
    virtual void allow(const system::hash_digest& public_key) NOEXCEPT;

    /// Allow clients with the following ip addresses (whitelist).
    virtual void allow(const authority& address) NOEXCEPT;

    /// Allow clients with the following ip addresses (blacklist).
    virtual void deny(const authority& address) NOEXCEPT;

protected:
    void work() NOEXCEPT override;

private:
    bool allowed_address(const std::string& address) const NOEXCEPT;
    bool allowed_key(const system::hash_digest& public_key) const NOEXCEPT;
    bool allowed_weak(const std::string& domain) const NOEXCEPT;

    // This is thread safe.
    context context_;

    // These are protected by mutex.
    bool require_allow_;
    sodium private_key_;
    std::unordered_set<system::hash_digest> keys_;
    std::unordered_set<std::string> weak_domains_;
    std::unordered_map<std::string, bool> adresses_;
    mutable std::shared_mutex property_mutex_;
    mutable std::shared_mutex stop_mutex_;
};

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif


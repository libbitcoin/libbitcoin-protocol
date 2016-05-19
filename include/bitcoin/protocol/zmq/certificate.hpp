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
#ifndef LIBBITCOIN_PROTOCOL_ZMQ_CERTIFICATE_HPP
#define LIBBITCOIN_PROTOCOL_ZMQ_CERTIFICATE_HPP

#include <string>
#include <bitcoin/protocol/define.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

/// A simplified "certificate" class to manage a curve key pair.
/// If valid the class always retains a consistent key pair.
class BCP_API certificate
{
public:
    /// Construct a new certificate (can we inject randomness).
    certificate();

    /// Construct a certificate from a private key (generates public key).
    certificate(const std::string& private_key);

    /// True if the certificate is valid.
    operator const bool() const;

    /// The public key base85 text.
    const std::string& public_key() const;

    /// The secret key base85 text.
    const std::string& private_key() const;

protected:
    void create(std::string& out_public, std::string& out_private);
    bool derive(std::string& out_public, const std::string& private_key);

private:
    std::string public_;
    std::string private_;
};

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif

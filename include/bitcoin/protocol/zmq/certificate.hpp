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
#ifndef LIBBITCOIN_PROTOCOL_ZMQ_CERTIFICATE_HPP
#define LIBBITCOIN_PROTOCOL_ZMQ_CERTIFICATE_HPP

#include <bitcoin/system.hpp>
#include <bitcoin/protocol/config/sodium.hpp>
#include <bitcoin/protocol/define.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

/// This class is not thread safe.
/// A simplified "certificate" class to manage a curve key pair.
/// If valid the class always retains a consistent key pair.
class BCP_API certificate
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(certificate);

    /// Construct an arbitary keypair as a new certificate.
    /// This always reduces keyspace, disallowing '#' in text encoding.
    /// Use certificate({ null_hash }) to allow full key space.
    certificate() NOEXCEPT;

    /// Construct a certificate from private key (generates public key).
    /// This generates an arbitary key pair if the parameter is uninitialized.
    certificate(const sodium& private_key) NOEXCEPT;

    /// True if the certificate is valid.
    operator bool() const NOEXCEPT;

    /// The public key base85 text.
    const sodium& public_key() const NOEXCEPT;

    /// The private key base85 text.
    const sodium& private_key() const NOEXCEPT;

protected:
    static bool derive(sodium& out_public, const sodium& private_key) NOEXCEPT;
    static bool create(sodium& out_public, sodium& out_private,
        bool setting) NOEXCEPT;

private:
    sodium public_;
    sodium private_;
};

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif

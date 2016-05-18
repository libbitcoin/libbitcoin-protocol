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

#include <map>
#include <string>
#include <utility>
#include <boost/filesystem.hpp>
#include <bitcoin/protocol/define.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

class BCP_API certificate
{
public:
    typedef std::pair<std::string, std::string> metadata;

    /// Contruct a new certificate (can we inject randomness).
    certificate();

    /// Contruct a certificate from the specified path.
    certificate(const boost::filesystem::path& path);

    /// True if the certificate is valid.
    operator const bool() const;

    /// The public key base85 text.
    const std::string& public_key() const;

    /// The secret key base85 text.
    const std::string& secret_key() const;

    /// Add medata to the certificate.
    void add_metadata(const metadata& metadata);

    /// Add medata to the certificate.
    void add_metadata(const std::string& name, const std::string& value);

    /// Export the public key to a certificate file.
    bool export_public(const boost::filesystem::path& path);

    /// Export the secret key to a certificate file.
    bool export_secret(const boost::filesystem::path& path);

    /// Load a certificate from the specified path (always replaces existing).
    bool load(const boost::filesystem::path& path);

private:
    typedef std::map<std::string, std::string> metadata_map;

    std::string public_;
    std::string secret_;
    metadata_map metadata_;
};

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif

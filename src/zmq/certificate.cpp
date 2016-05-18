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
#include <bitcoin/protocol/zmq/certificate.hpp>

#include <string>
#include <boost/filesystem.hpp>
////#include <zmq.h>
#include <bitcoin/protocol/zmq/socket.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

using path = boost::filesystem::path;

// Always generates both keys.
// Loop until neither key's base85 encoding includes the # character.
certificate::certificate()
{
    ////certificate_ = zcert_new();
}

// If the certificate is secret, generates the public key.
// If the certificate is public, does not set a secret key.
// If the file fails to parse then neither key is set (invalid).
certificate::certificate(const path& path)
{
    /* bool */ load(path);
}

certificate::operator const bool() const
{
    return !public_.empty();
}

const std::string& certificate::public_key() const
{
    return public_;
}

const std::string& certificate::secret_key() const
{
    return secret_;
}

void certificate::add_metadata(const metadata& metadata)
{
    metadata_.emplace(metadata);
    ////zcert_set_meta(certificate_, name.c_str(), value.c_str());
}

void certificate::add_metadata(const std::string& name,
    const std::string& value)
{
    add_metadata({ name, value });
    ////zcert_set_meta(certificate_, name.c_str(), value.c_str());
}

// The public certificate always excludes an existing secret key.
bool certificate::export_public(const path& path)
{
    return false;
    //// zcert_save_public(certificate_, filename.c_str());
}

// The secret certificate always contains a public key as well.
bool certificate::export_secret(const path& path)
{
    return false;
    //// zcert_save_secret(certificate_, filename.c_str());
}

bool certificate::load(const path& path)
{
    return false;
    ////certificate_ = zcert_load(filename.c_str());
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

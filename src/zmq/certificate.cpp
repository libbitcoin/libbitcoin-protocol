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
#include <zmq.h>
#include <bitcoin/bitcoin.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

static constexpr int32_t zmq_fail = -1;
static constexpr size_t zmq_encoded_key_size = 40;

certificate::certificate(bool setting)
{
    create(public_, private_, setting);
}

certificate::certificate(const hash_digest& private_key)
{
    std::string base85_private_key;

    // This cannot fail but we handle anyway.
    if (!encode_base85(base85_private_key, private_key))
        return;

    // If we successfully derive the public then set the private.
    if (derive(public_, base85_private_key))
        private_ = base85_private_key;
}

certificate::certificate(const std::string& base85_private_key)
{
    // If we successfully derive the public then set the private.
    if (derive(public_, base85_private_key))
        private_ = base85_private_key;
}

bool certificate::derive(std::string& out_public,
    const std::string& private_key)
{
    if (private_key.size() != zmq_encoded_key_size)
        return false;

    char public_key[zmq_encoded_key_size + 1] = { 0 };

    if (zmq_curve_public(public_key, private_key.data()) == zmq_fail)
        return false;

    out_public = public_key;
    return true;
}

bool certificate::create(std::string& out_public, std::string& out_private,
    bool setting)
{
    // TODO: update settings loader so this isn't necessary.
    // BUGBUG: this limitation weakens security by reducing key space.
    const auto ok_setting = [](const std::string& key)
    {
        return key.find_first_of('#') == std::string::npos;
    };

    // Loop until neither key's base85 encoding includes the # character.
    // This ensures that the value can be used in libbitcoin settings files.
    for (uint8_t attempt = 0; attempt <= max_uint8; attempt++)
    {
        char public_key[zmq_encoded_key_size + 1] = { 0 };
        char private_key[zmq_encoded_key_size + 1] = { 0 };

        if (zmq_curve_keypair(public_key, private_key) == zmq_fail)
            return false;

        if (!setting || ((ok_setting(public_key) && ok_setting(private_key))))
        {
            out_public = public_key;
            out_private = private_key;
            return true;
        }
    }

    return false;
}

certificate::operator const bool() const
{
    return !public_.empty();
}

const std::string& certificate::public_key() const
{
    return public_;
}

const std::string& certificate::private_key() const
{
    return private_;
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

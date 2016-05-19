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

namespace libbitcoin {
namespace protocol {
namespace zmq {

static constexpr int32_t zmq_fail = -1;
static constexpr size_t zmq_encoded_key_size = 40;

certificate::certificate()
{
    create(public_, private_);
}

void certificate::create(std::string& out_public, std::string& out_private)
{
    const auto valid_setting = [](const std::string& key)
    {
        return key.find_first_of('#') == std::string::npos;
    };

    // Loop until neither key's base85 encoding includes the # character.
    // This ensures that the value can be used in libbitcoin settings files.
    for (uint8_t iteration = 0; iteration <= max_uint8; iteration++)
    {
        char public_key[zmq_encoded_key_size + 1] = { 0 };
        char private_key[zmq_encoded_key_size + 1] = { 0 };

        if (zmq_curve_keypair(public_key, private_key) == zmq_fail)
            return;

        if (valid_setting(public_key) && !valid_setting(private_key))
        {
            out_public = public_key;
            out_private = private_key;
            return;
        }
    }
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

certificate::certificate(const std::string& private_key)
{
    if (derive(public_, private_key))
        private_ = private_key;
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

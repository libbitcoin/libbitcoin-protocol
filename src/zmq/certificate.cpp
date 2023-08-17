/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/protocol/zmq/certificate.hpp>

#include <string>
#include <bitcoin/system.hpp>
#include <bitcoin/protocol/zmq/zeromq.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

using namespace bc::system;

certificate::certificate() NOEXCEPT
{
    // HACK: restricted key space for use with config files.
    create(public_, private_, true);
}

// Full key space.
certificate::certificate(const sodium& private_key) NOEXCEPT
{
    if (!private_key)
    {
        // Full key space (may include '#' character in z85 encoding).
        create(public_, private_, false);
        return;
    }

    if (derive(public_, private_key))
        private_ = private_key;
}

bool certificate::derive(sodium& out_public, const sodium& private_key) NOEXCEPT
{
    if (!private_key)
        return false;

    const auto key = private_key.to_string();
    std::string public_key(zmq_encoded_key_size, 0);

    if (zmq_curve_public(public_key.data(), key.data()) == zmq_fail)
        return false;

    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    out_public = sodium(public_key);
    BC_POP_WARNING()
    return out_public;
}

// TODO: update settings loader so this isn't necessary.
// BUGBUG: this limitation weakens security by reducing key space.
static inline bool ok_setting(const std::string& key) NOEXCEPT
{
    return key.find_first_of('#') == std::string::npos;
}

bool certificate::create(sodium& out_public, sodium& out_private,
    bool setting) NOEXCEPT
{
    // Loop until neither key's base85 encoding includes the # character.
    // This ensures that the value can be used in libbitcoin settings files.
    for (auto attempt = zero; attempt < max_uint8; attempt++)
    {
        std::string public_key(zmq_encoded_key_size, 0);
        std::string private_key(zmq_encoded_key_size, 0);

        // SECURITY: this uses platform random number generation.
        if (zmq_curve_keypair(public_key.data(), private_key.data()) == zmq_fail)
            return false;

        if (!setting || (ok_setting(public_key.data()) &&
            ok_setting(private_key.data())))
        {
            BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
            out_public = sodium(public_key);
            out_private = sodium(private_key);
            BC_POP_WARNING()
            return out_public;
        }
    }

    return false;
}

certificate::operator bool() const NOEXCEPT
{
    return public_;
}

const sodium& certificate::public_key() const NOEXCEPT
{
    return public_;
}

const sodium& certificate::private_key() const NOEXCEPT
{
    return private_;
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

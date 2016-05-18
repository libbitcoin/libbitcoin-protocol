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

// TODO:
// Loop until neither key's base85 encoding includes the # character.
certificate::certificate()
{
}

// TODO:
// Validate the private key and create the public key from the private.
certificate::certificate(const std::string& private_key)
{
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

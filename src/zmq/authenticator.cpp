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
#include <bitcoin/protocol/zmq/authenticator.hpp>

#include <boost/filesystem.hpp>
////#include <zmq.h>
#include <bitcoin/bitcoin.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

using path = boost::filesystem::path;

authenticator::authenticator(context& context)
  : authenticator_(nullptr)
{
    ////authenticator_ = zauth_new(context.self());
}

authenticator::~authenticator()
{
    ////zauth_destroy(&authenticator_);
}

authenticator::operator const bool() const
{
    return authenticator_ != nullptr;
}

void authenticator::allow(const config::authority& address)
{
    ////zauth_allow(authenticator_, address.c_str());
}

void authenticator::deny(const config::authority& address)
{
    ////zauth_deny(authenticator_, address.c_str());
}

bool authenticator::certificates(const path& path)
{
    return false;
    // Clear requriement if empty.
    // Return false if the path cannot be created.
    // Declares that a client cert must be matched (unless empty).
    ////zauth_configure_curve(authenticator_, "*", path.c_str());
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

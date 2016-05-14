/*
 * Copyright (c) 2011-2013 libczmq++ developers (see AUTHORS)
 *
 * This file is part of libczmq++.
 *
 * libczmq++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
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

#include <czmq.h>

namespace libbitcoin {
namespace protocol {
namespace zmq {

authenticator::authenticator(context& ctx)
  : self_(zauth_new(ctx.self()))
{
}

authenticator::~authenticator()
{
    BITCOIN_ASSERT(self_);
    zauth_destroy(&self_);
}

authenticator::operator const bool() const
{
    return self_ != nullptr;
}

zauth_t* authenticator::self()
{
    return self_;
}

void authenticator::allow(const std::string& address)
{
    zauth_allow(self_, address.c_str());
}

void authenticator::deny(const std::string& address)
{
    zauth_deny(self_, address.c_str());
}

void authenticator::configure_plain(const std::string& domain,
    const std::string& filename)
{
    zauth_configure_plain(self_, domain.c_str(), filename.c_str());
}

void authenticator::configure_curve(const std::string& domain,
    const std::string& location)
{
    zauth_configure_curve(self_, domain.c_str(), location.c_str());
}

void authenticator::set_verbose(bool verbose)
{
    zauth_set_verbose(self_, verbose);
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

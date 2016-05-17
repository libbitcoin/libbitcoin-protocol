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

#include <zmq.h>

namespace libbitcoin {
namespace protocol {
namespace zmq {

authenticator::authenticator(context& context)
  : self_(nullptr)
{
    ////self_ = zauth_new(context.self());
}

authenticator::~authenticator()
{
    BITCOIN_ASSERT(self_);
    ////zauth_destroy(&self_);
}

authenticator::operator const bool() const
{
    return self_ != nullptr;
}

void* authenticator::self()
{
    return self_;
}

void authenticator::allow(const std::string& address)
{
    ////zauth_allow(self_, address.c_str());
}

void authenticator::deny(const std::string& address)
{
    ////zauth_deny(self_, address.c_str());
}

void authenticator::configure_plain(const std::string& domain,
    const std::string& filename)
{
    ////zauth_configure_plain(self_, domain.c_str(), filename.c_str());
}

void authenticator::configure_curve(const std::string& domain,
    const std::string& location)
{
    ////zauth_configure_curve(self_, domain.c_str(), location.c_str());
}

void authenticator::set_verbose(bool verbose)
{
#ifndef _MSC_VER
    // Hack to prevent czmq from writing to stdout/stderr on Windows.
    // This will prevent authentication feedback, but also prevent crashes.
    // It is necessary to prevent stdio when using our utf8-everywhere pattern.
    // TODO: drop czmq and use latest zmq to avoid hadcoded stdio logging.
    ////zauth_set_verbose(self_, verbose);
#endif
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

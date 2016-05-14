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
#ifndef LIBBITCOIN_PROTOCOL_ZMQ_CERTIFICATE_HPP
#define LIBBITCOIN_PROTOCOL_ZMQ_CERTIFICATE_HPP

#include <string>
#include <czmq.h>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/zmq/socket.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

class BCP_API certificate
{
public:
    certificate();
    certificate(zcert_t* self);
    certificate(certificate&& other);
    certificate(const std::string& filename);
    certificate(const certificate&) = delete;
    ~certificate();

    zcert_t* self();

    bool valid() const;
    void reset(zcert_t* self);
    void reset(const std::string& filename);
    void set_meta(const std::string& name, const std::string& value);
    int save(const std::string& filename);
    int save_public(const std::string& filename);
    int save_secret(const std::string& filename);
    std::string public_text() const;
    void apply(socket& sock);

private:
    zcert_t* self_;
};

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif

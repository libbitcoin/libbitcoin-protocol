/*
 * Copyright (c) 2011-2014 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin_protocol.
 *
 * libbitcoin_protocol is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBBITCOIN_PROTOCOL_MESSAGE
#define LIBBITCOIN_PROTOCOL_MESSAGE

#include <memory>
#include <czmq++/czmqpp.hpp>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/interface.pb.h>

namespace libbitcoin {
namespace protocol {

class BCP_API request_message
{
public:

    request_message();

    const bc::data_chunk origin() const;

    std::shared_ptr<request> get_request() const;

    bool receive(czmqpp::socket& socket);

private:

    bc::data_chunk origin_;

    std::shared_ptr<request> request_;
};


class BCP_API response_message
{
public:

    response_message();

    response_message(
        const bc::data_chunk& destination,
        std::shared_ptr<response> payload);

    void set_destination(const bc::data_chunk& destination);

    void set_response(std::shared_ptr<response> payload);

    bool reply(czmqpp::socket& socket);

private:

    bc::data_chunk destination_;

    std::shared_ptr<response> response_;
};

}
}

#endif

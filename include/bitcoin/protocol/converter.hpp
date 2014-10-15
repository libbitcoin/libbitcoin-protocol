/*
 * Copyright (c) 2011-2014 libbitcoin developers (see AUTHORS)
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBBITCOIN_PROTOCOL_CONVERSION_HPP
#define LIBBITCOIN_PROTOCOL_CONVERSION_HPP

#include <memory>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/interface.pb.h>

namespace libbitcoin {
namespace protocol {

class BCP_API converter
{
public:

    bool from_protocol(const point* point, bc::output_point& result);

    bool from_protocol(const std::shared_ptr<point> point,
        bc::output_point& result);

    bool from_protocol(const tx_input* input,
        bc::transaction_input_type& result);

    bool from_protocol(const std::shared_ptr<tx_input> input,
        bc::transaction_input_type& result);

    bool from_protocol(const tx_output* output,
        bc::transaction_output_type& result);

    bool from_protocol(const std::shared_ptr<tx_output> output,
        bc::transaction_output_type& result);

    bool from_protocol(const tx* transaction, bc::transaction_type& result);

    bool from_protocol(const std::shared_ptr<tx> transaction,
        bc::transaction_type& result);

    bool from_protocol(const block_header* header,
        bc::block_header_type& result);

    bool from_protocol(const std::shared_ptr<block_header> header,
        bc::block_header_type& result);

    bool from_protocol(const block* block, bc::block_type& result);

    bool from_protocol(const std::shared_ptr<block> block,
        bc::block_type& result);

    bool to_protocol(const bc::output_point& point,
        bc::protocol::point& result);

    bc::protocol::point* to_protocol(const bc::output_point& point);

    bool to_protocol(const bc::transaction_input_type& input, tx_input& result);

    tx_input* to_protocol(const bc::transaction_input_type& input);

    bool to_protocol(const bc::transaction_output_type& output, tx_output& result);

    tx_output* to_protocol(const bc::transaction_output_type& output);

    bool to_protocol(const bc::transaction_type& transaction, tx& result);

    tx* to_protocol(const bc::transaction_type& transaction);

    bool to_protocol(const bc::block_header_type& header, block_header& result);

    block_header* to_protocol(const bc::block_header_type& header);

    bool to_protocol(const bc::block_type& block, bc::protocol::block& result);

    block* to_protocol(const bc::block_type& block);
};

}
}

#endif

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

    virtual bool from_protocol(const point* point,
        bc::chain::output_point& result);

    virtual bool from_protocol(const std::shared_ptr<point> point,
        bc::chain::output_point& result);

    virtual bool from_protocol(const tx_input* input,
        bc::chain::transaction_input& result);

    virtual bool from_protocol(const std::shared_ptr<tx_input> input,
        bc::chain::transaction_input& result);

    virtual bool from_protocol(const tx_output* output,
        bc::chain::transaction_output& result);

    virtual bool from_protocol(const std::shared_ptr<tx_output> output,
        bc::chain::transaction_output& result);

    virtual bool from_protocol(const tx* transaction,
        bc::chain::transaction& result);

    virtual bool from_protocol(const std::shared_ptr<tx> transaction,
        bc::chain::transaction& result);

    virtual bool from_protocol(const block_header* header,
        bc::chain::block_header& result);

    virtual bool from_protocol(const std::shared_ptr<block_header> header,
        bc::chain::block_header& result);

    virtual bool from_protocol(const block* block, bc::chain::block& result);

    virtual bool from_protocol(const std::shared_ptr<block> block,
        bc::chain::block& result);

    virtual bool to_protocol(const bc::chain::output_point& point,
        bc::protocol::point& result);

    virtual bc::protocol::point* to_protocol(
        const bc::chain::output_point& point);

    virtual bool to_protocol(const bc::chain::transaction_input& input,
        tx_input& result);

    virtual tx_input* to_protocol(const bc::chain::transaction_input& input);

    virtual bool to_protocol(const bc::chain::transaction_output& output,
        tx_output& result);

    virtual tx_output* to_protocol(const bc::chain::transaction_output& output);

    virtual bool to_protocol(const bc::chain::transaction& transaction,
        tx& result);

    virtual tx* to_protocol(const bc::chain::transaction& transaction);

    virtual bool to_protocol(const bc::chain::block_header& header,
        block_header& result);

    virtual block_header* to_protocol(const bc::chain::block_header& header);

    virtual bool to_protocol(const bc::chain::block& block,
        bc::protocol::block& result);

    virtual block* to_protocol(const bc::chain::block& block);
};

}
}

#endif

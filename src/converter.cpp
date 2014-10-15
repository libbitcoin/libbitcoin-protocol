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

#include <bitcoin/protocol/converter.hpp>

namespace libbitcoin {
namespace protocol {

bool converter::from_protocol(const point* point, bc::output_point& result)
{
    bool success = false;

    if (point)
    {
        result.index = point->index();
        result.hash = bc::decode_hash(point->hash());
        success = (result.hash != null_hash);
    }

    return success;
}

bool converter::from_protocol(const std::shared_ptr<point> point,
    bc::output_point& result)
{
    return from_protocol(point.get(), result);
}

bool converter::from_protocol(const tx_input* input,
    bc::transaction_input_type& result)
{
    bool success = false;

    if (input)
    {
        success = true;

        bc::output_point previous;

        success = input->has_previous_output();

        if (success)
        {
            success = from_protocol(&(input->previous_output()), previous);
        }

        if (success)
        {
            success = input->has_script();
        }

        if (success)
        {
            std::string script_text = input->script();
            result.script = bc::parse_script(
                bc::data_chunk(script_text.begin(), script_text.end()));

            result.previous_output = previous;
            result.sequence = input->sequence();
        }
    }

    return success;
}

bool converter::from_protocol(const std::shared_ptr<tx_input> input,
    bc::transaction_input_type& result)
{
    return from_protocol(input.get(), result);
}

bool converter::from_protocol(const tx_output* output,
    bc::transaction_output_type& result)
{
    bool success = false;

    if (output && output->has_script())
    {
        success = true;

        std::string script_text = output->script();
        result.script = bc::parse_script(
            bc::data_chunk(script_text.begin(), script_text.end()));

        result.value = output->value();
    }

    return success;
}

bool converter::from_protocol(const std::shared_ptr<tx_output> output,
    bc::transaction_output_type& result)
{
    return from_protocol(output.get(), result);
}

bool converter::from_protocol(const tx* transaction,
    bc::transaction_type& result)
{
    bool success = false;

    if (transaction)
    {
        success = true;

        result.version = transaction->version();
        result.locktime = transaction->locktime();

        for (auto input : transaction->inputs())
        {
            bc::transaction_input_type bitcoin_input;

            if (!success || !from_protocol(&input, bitcoin_input))
            {
                success = false;
                break;
            }

            result.inputs.push_back(bitcoin_input);
        }

        for (auto output : transaction->outputs())
        {
            bc::transaction_output_type bitcoin_output;

            if (!success || !from_protocol(&output, bitcoin_output))
            {
                success = false;
                break;
            }

            result.outputs.push_back(bitcoin_output);
        }

        if (!success)
        {
            result.version = 0;
            result.locktime = 0;
            result.inputs.clear();
            result.outputs.clear();
        }
    }

    return success;
}

bool converter::from_protocol(const std::shared_ptr<tx> transaction,
    bc::transaction_type& result)
{
    return from_protocol(transaction.get(), result);
}

bool converter::from_protocol(const block_header* header,
    bc::block_header_type& result)
{
    bool success = false;

    if (header)
    {
        result.version = header->version();
        result.timestamp = header->timestamp();
        result.bits = header->bits();
        result.nonce = header->nonce();

        result.merkle = bc::decode_hash(header->merkle_root());

        result.previous_block_hash = bc::decode_hash(
            header->previous_block_hash());

        success = (result.merkle != null_hash)
            && (result.previous_block_hash != null_hash);
    }

    return success;
}

bool converter::from_protocol(const std::shared_ptr<block_header> header,
    bc::block_header_type& result)
{
    return from_protocol(header.get(), result);
}

bool converter::from_protocol(const block* block,
    block_type& result)
{
    bool success = false;

    if (block)
    {
        success = from_protocol(&(block->header()), result.header);

        if (success)
        {
            for (auto tx : block->transactions())
            {
                bc::transaction_type bitcoin_tx;

                if (!from_protocol(&tx, bitcoin_tx))
                {
                    success = false;
                    break;
                }

                result.transactions.push_back(bitcoin_tx);
            }
        }

        if (!success)
        {
            result.transactions.clear();
        }
    }

    return success;
}

bool converter::from_protocol(const std::shared_ptr<block> block,
    block_type& result)
{
    return from_protocol(block.get(), result);
}

}
}

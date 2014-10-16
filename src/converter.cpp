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
            result.script = bc::raw_data_script(
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
        result.script = bc::raw_data_script(
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

        if (success)
        {
            for (auto input : transaction->inputs())
            {
                bc::transaction_input_type bitcoin_input;

                if (!from_protocol(&input, bitcoin_input))
                {
                    success = false;
                    break;
                }

                result.inputs.push_back(bitcoin_input);
            }
        }

        if (success)
        {
            for (auto output : transaction->outputs())
            {
                bc::transaction_output_type bitcoin_output;

                if (!from_protocol(&output, bitcoin_output))
                {
                    success = false;
                    break;
                }

                result.outputs.push_back(bitcoin_output);
            }
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

bool converter::to_protocol(const bc::output_point& point,
    bc::protocol::point& result)
{
    bool success = true;

    result.set_hash(encode_hex(point.hash));
    result.set_index(point.index);

    return success;
}

point* converter::to_protocol(const bc::output_point& point)
{
    std::unique_ptr<bc::protocol::point> result(new bc::protocol::point());

    if (!to_protocol(point, *(result.get())))
    {
        result.reset();
    }

    return result.release();
}

bool converter::to_protocol(const bc::transaction_input_type& input,
    tx_input& result)
{
    bool success = true;

    result.set_allocated_previous_output(to_protocol(input.previous_output));

    bc::data_chunk script_data = bc::save_script(input.script);
    result.set_script(std::string(script_data.begin(), script_data.end()));

    result.set_sequence(input.sequence);

    return success;
}

tx_input* converter::to_protocol(const bc::transaction_input_type& input)
{
    std::unique_ptr<tx_input> result(new tx_input());

    if (!to_protocol(input, *(result.get())))
    {
        result.reset();
    }

    return result.release();
}

bool converter::to_protocol(const bc::transaction_output_type& output,
    tx_output& result)
{
    bool success = true;

    result.set_value(output.value);

    bc::data_chunk script_data = bc::save_script(output.script);
    result.set_script(std::string(script_data.begin(), script_data.end()));

    return success;
}

tx_output* converter::to_protocol(const bc::transaction_output_type& output)
{
    std::unique_ptr<tx_output> result(new tx_output());

    if (!to_protocol(output, *(result.get())))
    {
        result.reset();
    }

    return result.release();
}

bool converter::to_protocol(const bc::transaction_type& transaction, tx& result)
{
    bool success = true;

    result.set_version(transaction.version);
    result.set_locktime(transaction.locktime);

    google::protobuf::RepeatedPtrField<tx_input>* repeated_inputs
        = result.mutable_inputs();

    if (success)
    {
        for (auto input : transaction.inputs)
        {
            if (!to_protocol(input, *(repeated_inputs->Add())))
            {
                success = false;
                break;
            }
        }
    }

    google::protobuf::RepeatedPtrField<tx_output>* repeated_outputs
        = result.mutable_outputs();

    if (success)
    {
        for (auto output : transaction.outputs)
        {
            if (!to_protocol(output, *(repeated_outputs->Add())))
            {
                success = false;
                break;
            }
        }
    }

    if (!success)
    {
        result.clear_version();
        result.clear_locktime();
        result.clear_inputs();
        result.clear_outputs();
    }

    return success;
}

tx* converter::to_protocol(const bc::transaction_type& transaction)
{
    std::unique_ptr<tx> result(new tx());

    if (!to_protocol(transaction, *(result.get())))
    {
        result.reset();
    }

    return result.release();
}

bool converter::to_protocol(const bc::block_header_type& header,
    block_header& result)
{
    bool success = true;

    result.set_version(header.version);
    result.set_timestamp(header.timestamp);
    result.set_bits(header.bits);
    result.set_nonce(header.nonce);

    result.set_merkle_root(encode_hex(header.merkle));

    result.set_previous_block_hash(encode_hex(header.previous_block_hash));

    return success;
}

block_header* converter::to_protocol(const bc::block_header_type& header)
{
    std::unique_ptr<block_header> result(new block_header());

    if (!to_protocol(header, *(result.get())))
    {
        result.reset();
    }

    return result.release();
}

bool converter::to_protocol(const bc::block_type& block,
    bc::protocol::block& result)
{
    bool success = true;

    result.set_allocated_header(to_protocol(block.header));

    success = result.has_header();

    google::protobuf::RepeatedPtrField<tx>* repeated_transactions
        = result.mutable_transactions();

    if (success)
    {
        for (auto transaction : block.transactions)
        {
            if (!to_protocol(transaction, *(repeated_transactions->Add())))
            {
                success = false;
                break;
            }
        }
    }

    if (!success)
    {
        result.clear_header();
        result.clear_transactions();
    }

    return success;
}

bc::protocol::block* converter::to_protocol(const bc::block_type& block)
{
    std::unique_ptr<bc::protocol::block> result(new bc::protocol::block());

    if (!to_protocol(block, *(result.get())))
    {
        result.reset();
    }

    return result.release();
}

}
}

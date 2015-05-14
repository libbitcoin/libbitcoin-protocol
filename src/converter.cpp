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

/**
 * Copy `binary` data from protobuf's storage format (std::string)
 * to libbitcoin's storage format (hash_digest).
 */
static bool unpack_hash(hash_digest& out, const std::string& in)
{
    if (in.size() != hash_size)
        return false;
    std::copy(in.begin(), in.end(), out.begin());
    return true;
}

static std::string pack_hash(hash_digest in)
{
    return std::string(in.begin(), in.end());
}

bool converter::from_protocol(const point* point,
    bc::chain::output_point& result)
{
    bool success = false;

    if (point)
    {
        result.index = point->index();
        success = unpack_hash(result.hash, point->hash());
    }

    return success;
}

bool converter::from_protocol(const std::shared_ptr<point> point,
    bc::chain::output_point& result)
{
    return from_protocol(point.get(), result);
}

bool converter::from_protocol(const tx_input* input,
    bc::chain::transaction_input& result)
{
    bool success = false;

    if (input)
    {
        success = true;

        bc::chain::output_point previous;

        success = input->has_previous_output();

        if (success)
            success = from_protocol(&(input->previous_output()), previous);

        if (success)
            success = input->has_script();

        if (success)
        {
            result.previous_output = previous;
            result.sequence = input->sequence();

            std::string script_text = input->script();

            // protocol question - is the data encoding of the script to be prefixed with operation count?
            success = result.script.from_data(bc::data_chunk(
                script_text.begin(), script_text.end()), false, true);
        }
    }

    return success;
}

bool converter::from_protocol(const std::shared_ptr<tx_input> input,
    bc::chain::transaction_input& result)
{
    return from_protocol(input.get(), result);
}

bool converter::from_protocol(const tx_output* output,
    bc::chain::transaction_output& result)
{
    bool success = false;

    if (output && output->has_script())
    {
        success = true;

        result.value = output->value();

        std::string script_text = output->script();

        // protocol question - is the data encoding of the script to be prefixed with operation count?
        success = result.script.from_data(bc::data_chunk(
            script_text.begin(), script_text.end()), false, true);
    }

    return success;
}

bool converter::from_protocol(const std::shared_ptr<tx_output> output,
    bc::chain::transaction_output& result)
{
    return from_protocol(output.get(), result);
}

bool converter::from_protocol(const tx* transaction,
    bc::chain::transaction& result)
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
                bc::chain::transaction_input bitcoin_input;

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
                bc::chain::transaction_output bitcoin_output;

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
    bc::chain::transaction& result)
{
    return from_protocol(transaction.get(), result);
}

bool converter::from_protocol(const block_header* header,
    bc::chain::block_header& result)
{
    bool success = false;

    if (header)
    {
        result.version = header->version();
        result.timestamp = header->timestamp();
        result.bits = header->bits();
        result.nonce = header->nonce();

        success = unpack_hash(result.merkle, header->merkle_root());

        success &= unpack_hash(result.previous_block_hash,
            header->previous_block_hash());
    }

    return success;
}

bool converter::from_protocol(const std::shared_ptr<block_header> header,
    bc::chain::block_header& result)
{
    return from_protocol(header.get(), result);
}

bool converter::from_protocol(const block* block,
    bc::chain::block& result)
{
    bool success = false;

    if (block)
    {
        success = from_protocol(&(block->header()), result.header);

        if (success)
        {
            for (auto tx : block->transactions())
            {
                bc::chain::transaction bitcoin_tx;

                if (!from_protocol(&tx, bitcoin_tx))
                {
                    success = false;
                    break;
                }

                result.transactions.push_back(bitcoin_tx);
            }
        }

        if (!success)
            result.transactions.clear();
    }

    return success;
}

bool converter::from_protocol(const std::shared_ptr<block> block,
    bc::chain::block& result)
{
    return from_protocol(block.get(), result);
}

bool converter::to_protocol(const bc::chain::output_point& point,
    bc::protocol::point& result)
{
    bool success = true;

    result.set_hash(pack_hash(point.hash));
    result.set_index(point.index);

    return success;
}

point* converter::to_protocol(const bc::chain::output_point& point)
{
    std::unique_ptr<bc::protocol::point> result(new bc::protocol::point());

    if (!to_protocol(point, *(result.get())))
        result.reset();

    return result.release();
}

bool converter::to_protocol(const bc::chain::transaction_input& input,
    tx_input& result)
{
    bool success = true;

    auto previous_output = input.previous_output;
    result.set_allocated_previous_output(to_protocol(previous_output));

    // protocol question - is the data encoding of the script to be prefixed with operation count?
    bc::data_chunk script_data = input.script.to_data(false);
    result.set_script(std::string(script_data.begin(), script_data.end()));

    result.set_sequence(input.sequence);

    return success;
}

tx_input* converter::to_protocol(const bc::chain::transaction_input& input)
{
    std::unique_ptr<tx_input> result(new tx_input());

    if (!to_protocol(input, *(result.get())))
        result.reset();

    return result.release();
}

bool converter::to_protocol(const bc::chain::transaction_output& output,
    tx_output& result)
{
    bool success = true;

    result.set_value(output.value);

    // protocol question - is the data encoding of the script to be prefixed with operation count?
    bc::data_chunk script_data = output.script.to_data(false);
    result.set_script(std::string(script_data.begin(), script_data.end()));

    return success;
}

tx_output* converter::to_protocol(const bc::chain::transaction_output& output)
{
    std::unique_ptr<tx_output> result(new tx_output());

    if (!to_protocol(output, *(result.get())))
        result.reset();

    return result.release();
}

bool converter::to_protocol(const bc::chain::transaction& transaction, tx& result)
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

tx* converter::to_protocol(const bc::chain::transaction& transaction)
{
    std::unique_ptr<tx> result(new tx());

    if (!to_protocol(transaction, *(result.get())))
        result.reset();

    return result.release();
}

bool converter::to_protocol(const bc::chain::block_header& header,
    block_header& result)
{
    bool success = true;

    result.set_version(header.version);
    result.set_timestamp(header.timestamp);
    result.set_bits(header.bits);
    result.set_nonce(header.nonce);
    result.set_merkle_root(pack_hash(header.merkle));
    result.set_previous_block_hash(pack_hash(header.previous_block_hash));

    return success;
}

block_header* converter::to_protocol(const bc::chain::block_header& header)
{
    std::unique_ptr<block_header> result(new block_header());

    if (!to_protocol(header, *(result.get())))
        result.reset();

    return result.release();
}

bool converter::to_protocol(const bc::chain::block& block,
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

bc::protocol::block* converter::to_protocol(const bc::chain::block& block)
{
    std::unique_ptr<bc::protocol::block> result(new bc::protocol::block());

    if (!to_protocol(block, *(result.get())))
        result.reset();

    return result.release();
}

}
}

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

#include <bitcoin/protocol/message.hpp>

#include <string>

namespace libbitcoin {
namespace protocol {

//
// message base implementation
//
message::message()
{
}

message::~message()
{
}

const bc::data_chunk message::destination() const
{
    return destination_;
}

void message::set_destination(const bc::data_chunk& destination)
{
    destination_ = destination;
}

const bc::data_chunk message::origin() const
{
    return origin_;
}

bool message::receive(czmqpp::socket& socket)
{
    bool requires_reply = false;
    czmqpp::message message;

    requires_reply = message.receive(socket);

    if (requires_reply)
    {
        const data_stack& parts = message.parts();

        if (parts.size() > 0)
        {
            auto it = parts.begin();

            // Extract reply address(es)
            if (parts.size() > 2)
            {
                origin_ = *it;
                ++it;
            }

            // Confirm empty delimiter frame
            ++it;

            // Parse request payload from data frame
            if (decode_payload(*it))
            {
                ++it;

                // Confirm end of message (unnecessary)
                BITCOIN_ASSERT(it == parts.end());
            }
        }
    }

    return requires_reply;
}

bool message::receive(const std::shared_ptr<czmqpp::socket>& socket)
{
    bool result = false;

    if (socket)
        result = receive(*(socket.get()));

    return result;
}

bool message::send(czmqpp::socket& socket)
{
    bool result = false;

    czmqpp::message message;

    if (!destination_.empty())
        message.append(destination_);

    czmqpp::data_chunk empty;
    message.append(empty);

    if (encode_payload(message))
    {
        message.send(socket);
        result = true;
    }

    return result;
}

bool message::send(const std::shared_ptr<czmqpp::socket>& socket)
{
    bool result = false;

    if (socket)
        result = send(*(socket.get()));

    return result;
}


//
// request implementation
//
request_message::request_message()
: request_(nullptr)
{
}

request_message::~request_message()
{
}

std::shared_ptr<request> request_message::get_request() const
{
    return request_;
}

void request_message::set_request(std::shared_ptr<request> payload)
{
    request_ = payload;
}

bool request_message::encode_payload(czmqpp::message& message) const
{
    bool result = false;

    if (request_)
    {
        std::string data = request_->SerializeAsString();
        message.append(czmqpp::data_chunk(data.begin(), data.end()));
        result = true;
    }

    return result;
}

bool request_message::decode_payload(const bc::data_chunk& payload)
{
    bool result = false;

    std::string text = std::string(payload.begin(), payload.end());
    std::shared_ptr<request> data(new request());

    if (data->ParseFromString(text))
    {
        request_ = data;
        result = true;
    }

    return result;
}


//
// response implementation
//
response_message::response_message()
: response_(nullptr) 
{
}

response_message::~response_message()
{
}

std::shared_ptr<response> response_message::get_response() const
{
    return response_;
}

void response_message::set_response(std::shared_ptr<response> payload)
{
    response_ = payload;
}

bool response_message::encode_payload(czmqpp::message& message) const
{
    bool result = false;

    if (response_)
    {
        std::string data = response_->SerializeAsString();
        message.append(czmqpp::data_chunk(data.begin(), data.end()));
        result = true;
    }

    return result;
}

bool response_message::decode_payload(const bc::data_chunk& payload)
{
    bool result = false;

    std::string text = std::string(payload.begin(), payload.end());
    std::shared_ptr<response> data(new response());

    if (data->ParseFromString(text))
    {
        response_ = data;
        result = true;
    }

    return result;
}

}
}


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

bool message::send(czmqpp::socket& socket)
{
    bool result = false;

    czmqpp::message message;

    if (!destination_.empty())
    {
        message.append(destination_);
    }

    czmqpp::data_chunk empty;
    message.append(empty);

    if (encode_payload(message))
    {
        message.send(socket);
        result = true;
    }

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

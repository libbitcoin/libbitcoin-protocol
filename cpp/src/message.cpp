
#include <bitcoin/protocol/message.hpp>

namespace libbitcoin {
namespace protocol {

request_message::request_message()
: request_(nullptr)
{
}

const bc::data_chunk request_message::origin() const
{
    return origin_;
}

std::shared_ptr<request> request_message::get_request() const
{
    return request_;
}

bool request_message::receive(czmqpp::socket& socket)
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
            std::string payload = std::string((*it).begin(), (*it).end());
            std::shared_ptr<request> request(new request());

            if (request->ParseFromString(payload))
            {
                request_ = request;
            }

            ++it;

            // Confirm end of message (unnecessary)
            BITCOIN_ASSERT(it == parts.end());
        }
    }

    return requires_reply;
}



response_message::response_message()
: response_(nullptr) 
{
}

response_message::response_message(
    const bc::data_chunk& destination,
    std::shared_ptr<response> payload)
: destination_(destination), response_(payload)
{
}

void response_message::set_destination(const bc::data_chunk& destination)
{
    destination_ = destination;
}

void response_message::set_response(std::shared_ptr<response> payload)
{
    response_ = payload;
}

bool response_message::reply(czmqpp::socket& socket)
{
    czmqpp::message message;

    if (!destination_.empty())
    {
        message.append(destination_);
    }

    czmqpp::data_chunk empty;
    message.append(empty);

    std::string data = response_->SerializeAsString();
    message.append(czmqpp::data_chunk(data.begin(), data.end()));

    message.send(socket);

    return true;
}

}
}

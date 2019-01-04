/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_PROTOCOL_WEB_CONNECTION_HPP
#define LIBBITCOIN_PROTOCOL_WEB_CONNECTION_HPP

#ifdef _MSC_VER
#include <windows.h>
#endif

#include <algorithm>
#include <cstdint>
#include <functional>
#include <set>
#include <string>
#include <memory>
#include <vector>
#include <bitcoin/system.hpp>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/web/connection_state.hpp>
#include <bitcoin/protocol/web/event.hpp>
#include <bitcoin/protocol/web/file_transfer.hpp>
#include <bitcoin/protocol/web/http.hpp>
#include <bitcoin/protocol/web/ssl.hpp>
#include <bitcoin/protocol/web/utilities.hpp>
#include <bitcoin/protocol/web/websocket_frame.hpp>
#include <bitcoin/protocol/web/websocket_op.hpp>
#include <bitcoin/protocol/web/websocket_transfer.hpp>

namespace libbitcoin {
namespace protocol {
namespace http {

class connection;

// TODO: make internal connection typedefs.
typedef std::shared_ptr<connection> connection_ptr;
typedef std::set<connection_ptr> connection_set;
typedef std::vector<connection_ptr> connection_list;
typedef std::function<bool(connection_ptr, event, void* data)> event_handler;

// This class is instantiated from accepted/incoming HTTP clients.
// Initiating outgoing HTTP connections are not currently supported.
class BCP_API connection
{
public:
    typedef std::function<int32_t(const uint8_t*, size_t)> write_method;

    connection();
    connection(sock_t connection, const sockaddr_in& address);
    ~connection();

    // Properties.
    // ------------------------------------------------------------------------

    connection_state state() const;
    void set_state(connection_state state);

    void* user_data();
    void set_user_data(void* user_data);

    bool websocket() const;
    void set_websocket(bool websocket);

    bool json_rpc() const;
    void set_json_rpc(bool json_rpc);

    // The connection endpoint is a request uri, such as '/'.
    const std::string& uri() const;
    void set_uri(const std::string& uri);

    // Readers and Writers.
    // ------------------------------------------------------------------------
    // Signed integer results overload negative range for error code.

    http::read_buffer& read_buffer();
    system::data_chunk& write_buffer();

    int32_t read();
    int32_t read_length();

    int32_t write(const system::data_chunk& buffer);
    int32_t write(const std::string& buffer);
    int32_t write(const uint8_t* data, size_t length);

    int32_t unbuffered_write(const system::data_chunk& buffer);
    int32_t unbuffered_write(const std::string& buffer);
    int32_t unbuffered_write(const uint8_t* data, size_t length);

    // Other.
    // ------------------------------------------------------------------------

    void set_socket_non_blocking();
    sockaddr_in address() const;
    bool reuse_address() const;
    bool closed() const;
    void close();
    sock_t& socket();
    http::ssl& ssl_context();
    bool ssl_enabled() const;

    http::file_transfer& file_transfer();
    http::websocket_transfer& websocket_transfer();

    // Operator overloads.
    // ------------------------------------------------------------------------

    bool operator==(const connection& other);

private:
    void* user_data_;
    connection_state state_;
    sock_t socket_;
    sockaddr_in address_;
    system::asio::time_point last_active_;
    ssl ssl_context_;
    std::string uri_;
    bool websocket_;
    bool json_rpc_;

    // Transfer states used for read continuations, particularly for when the
    // read_buffer_ size is too small to hold all of the incoming data.
    http::file_transfer file_transfer_;
    http::websocket_transfer websocket_transfer_;

    int32_t bytes_read_;
    http::read_buffer read_buffer_;
    system::data_chunk write_buffer_;
};

} // namespace http
} // namespace protocol
} // namespace libbitcoin

#endif

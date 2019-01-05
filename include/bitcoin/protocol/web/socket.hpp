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
#ifndef LIBBITCOIN_PROTOCOL_WEB_SOCKET_HPP
#define LIBBITCOIN_PROTOCOL_WEB_SOCKET_HPP

#include <cstdint>
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <boost/filesystem.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/system.hpp>
#include <bitcoin/protocol.hpp>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/zmq/worker.hpp>
#include <bitcoin/protocol/web/connection.hpp>
#include <bitcoin/protocol/web/event.hpp>
#include <bitcoin/protocol/web/http.hpp>
#include <bitcoin/protocol/web/http_reply.hpp>
#include <bitcoin/protocol/web/json_string.hpp>
#include <bitcoin/protocol/web/manager.hpp>
#include <bitcoin/protocol/web/utilities.hpp>
#include <bitcoin/protocol/web/websocket_message.hpp>

namespace libbitcoin {
namespace protocol {
namespace http {

class BCP_API socket
  : public zmq::worker
{
public:
    class query_response_task
    {
      public:
        virtual ~query_response_task() = default;
        virtual bool run() = 0;
    };

    typedef std::shared_ptr<query_response_task> query_response_task_ptr;
    typedef std::vector<query_response_task_ptr> query_response_task_list;

    // Handles translation of incoming JSON to zmq protocol methods and
    // converting the result back to JSON for web clients.
    struct handlers
    {
        typedef std::function<bool(bc::protocol::zmq::message&,
            const std::string&, const std::string&, uint32_t)> encode_handler;
        typedef std::function<void(const system::data_chunk&, uint32_t,
            connection_ptr)> decode_handler;

        std::string command;
        encode_handler encode;
        decode_handler decode;
    };

    typedef std::unordered_map<std::string, handlers> handler_map;

    // Tracks websocket queries via the query_work_map. Used for matching
    // websocket client requests to zmq query responses.
    struct query_work_item
    {
        uint32_t id;
        uint32_t correlation_id;
        connection_ptr connection;
        std::string command;
        std::string arguments;
    };

    typedef std::unordered_map<uint32_t, std::pair<connection_ptr, uint32_t>>
        query_correlation_map;

    typedef std::unordered_map<uint32_t, query_work_item> query_work_map;
    typedef std::unordered_map<connection_ptr, query_work_map>
        connection_work_map;

    /// Construct a socket class.
    socket(bc::protocol::zmq::context& context,
        const bc::protocol::settings& settings, bool secure);

    /// Start the service.
    bool start() override;

    void queue_response(uint32_t sequence, const system::data_chunk& data,
        const std::string& command);
    bool send_query_responses();

    size_t connection_count() const;
    void add_connection(connection_ptr connection);
    void remove_connection(connection_ptr connection);
    void notify_query_work(connection_ptr connection,
        const std::string& method, uint32_t id, const std::string& parameters);

protected:
    // Initialize the websocket event loop and start a thread to poll events.
    virtual bool start_websocket_handler();

    // Terminate the websocket event loop.
    virtual bool stop_websocket_handler();

    virtual void handle_websockets();

    virtual const system::config::endpoint& zeromq_endpoint() const = 0;
    virtual const system::config::endpoint& websocket_endpoint() const = 0;
    virtual const std::shared_ptr<bc::protocol::zmq::socket> service() const;

    // Send a message to the websocket client.
    void send(connection_ptr connection, const std::string& json);

    // Send a message to every connected websocket client.
    void broadcast(const std::string& json);

    // Optionally set a fixed reply instead of returning a 404 not found if
    // the web root cannot be found.
    void set_default_page_data(const std::string& data);

    // The zmq socket operates on only this one thread.
    bc::protocol::zmq::context& context_;
    const bool secure_;
    const std::string security_;
    const bc::protocol::settings& settings_;

    // Both handlers_ and rpc_handlers_ are effectively const.
    handler_map handlers_;
    handler_map rpc_handlers_;

    // For query socket, service() is used to retrieve the zmq socket
    // connected to the query_socket service.  This socket operates on
    // only the below member thread_.
    std::shared_ptr<system::asio::thread> thread_;
    std::promise<bool> socket_started_;

    // Used by the query_socket class.
    uint32_t sequence_;
    connection_work_map work_;
    query_correlation_map correlations_;

private:
    static bool handle_event(connection_ptr connection, http::event event,
        const void* data);

    manager::ptr manager_;

    // This is protected by mutex.
    query_response_task_list query_response_tasks_;
    system::shared_mutex query_response_task_mutex_;
};

} // namespace http
} // namespace protocol
} // namespace libbitcoin

#endif

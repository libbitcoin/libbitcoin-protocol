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
#ifndef LIBBITCOIN_PROTOCOL_WEB_MANAGER_HPP
#define LIBBITCOIN_PROTOCOL_WEB_MANAGER_HPP

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <bitcoin/system.hpp>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/web/bind_options.hpp>
#include <bitcoin/protocol/web/connection.hpp>
#include <bitcoin/protocol/web/http.hpp>
#include <bitcoin/protocol/web/http_reply.hpp>
#include <bitcoin/protocol/web/http_request.hpp>
#include <bitcoin/protocol/web/utilities.hpp>
#include <bitcoin/protocol/web/websocket_frame.hpp>
#include <bitcoin/protocol/web/websocket_message.hpp>
#include <bitcoin/protocol/web/websocket_op.hpp>

#ifdef WITH_MBEDTLS
extern "C"
{
// Random data generator used by mbedtls for SSL.
int https_random(void*, unsigned char* buffer, size_t length);
}
#endif

namespace libbitcoin {
namespace protocol {
namespace http {

class BCP_API manager
{
public:
    class task
    {
      public:
        virtual ~task() = default;
        virtual bool run() = 0;
        virtual connection_ptr connection() = 0;
    };

    typedef boost::filesystem::path path;
    typedef std::shared_ptr<task> task_ptr;
    typedef std::vector<task_ptr> task_list;
    typedef std::shared_ptr<manager> ptr;
    typedef std::function<void()> handler;
    typedef std::vector<std::string> origin_list;

    manager(bool ssl, event_handler handler, path document_root,
        const origin_list origins);
    ~manager();

    // If the endpoint is reached and no default page can be found,
    // send a response that includes this data.
    void set_default_page_data(const std::string& data);

    bool initialize();
    bool bind(const system::config::endpoint& address,
        const bind_options& options);

    // Connections.
    bool accept_connection();
    void add_connection(connection_ptr connection);
    void remove_connection(connection_ptr connection);
    size_t connection_count() const;

    bool ssl() const;
    bool listening() const;
    bool stopped() const;

    void start();
    // Same as start above, but allows a callback to be called after
    // each iteration of run_once.
    void start(handler callback);
    void stop();
    void execute(task_ptr task);
    void run_tasks();

    void poll(size_t timeout_milliseconds);
    bool handle_connection(connection_ptr connection, event current_event);

private:
#ifdef WITH_MBEDTLS
    // Passed to mbedtls for internal use only.
    static int32_t ssl_send(void* data, const uint8_t* buffer, size_t length);
    static int32_t ssl_receive(void* data, uint8_t* buffer, size_t length);
#endif

    void run_once();
    void select(size_t timeout_milliseconds, connection_list& sockets);
    bool transfer_file_data(connection_ptr connection);
    bool send_http_file(connection_ptr connection, const path& path, bool keep_alive);
    bool handle_websocket(connection_ptr connection);
    bool send_response(connection_ptr connection, const http_request& request);
    bool send_generated_reply(connection_ptr connection, protocol_status status);
    bool upgrade_connection(connection_ptr connection, const http_request& request);
    bool validate_origin(const std::string& origin);
    bool initialize_ssl(connection_ptr connection, bool listener);

    // These are thread safe.
    const bool ssl_;
    std::atomic<bool> running_;
    std::atomic<bool> listening_;

    // Initialize is not thread safe.
    bool initialized_;

    // Bind is not thread safe.
    uint16_t port_;

    void* user_data_;
    path key_;
    path certificate_;
    path ca_certificate_;
    event_handler handler_;
    path document_root_;
    connection_list connections_;
    connection_ptr listener_;
    sockaddr_in listener_address_;

    // This is protected by mutex.
    task_list tasks_;
    system::shared_mutex task_mutex_;

    const origin_list origins_;
    std::string page_data_;
};

} // namespace http
} // namespace protocol
} // namespace libbitcoin

#endif

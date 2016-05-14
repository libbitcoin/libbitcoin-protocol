/////**
//// * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
//// *
//// * This file is part of libbitcoin-protocol.
//// *
//// * libbitcoin-protocol is free software: you can redistribute it and/or
//// * modify it under the terms of the GNU Affero General Public License with
//// * additional permissions to the one published by the Free Software
//// * Foundation, either version 3 of the License, or (at your option)
//// * any later version. For more information see LICENSE.
//// *
//// * This program is distributed in the hope that it will be useful,
//// * but WITHOUT ANY WARRANTY; without even the implied warranty of
//// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//// * GNU Affero General Public License for more details.
//// *
//// * You should have received a copy of the GNU Affero General Public License
//// * along with this program. If not, see <http://www.gnu.org/licenses/>.
//// */
////#include <string>
////#include <thread>
////#include <czmq.h>
////#include <bitcoin/protocol.hpp>
////
////using namespace bc;
////using namespace bc::protocol;
////
////int main()
////{
////    auto test_cb = [](zmq::socket& pipe)
////    {
////        auto ping = zstr_recv(pipe.self());
////        zstr_free(&ping);
////        zstr_send(pipe.self(), "pong");
////    };
////
////    zmq::context context;
////    assert(context.self());
////
////    auto pipe = thread_fork(context, test_cb);
////    assert(pipe.self());
////
////    zstr_send(pipe.self(), "ping");
////    auto pong = zstr_recv(pipe.self());
////    assert(streq(pong, "pong"));
////
////    zstr_free(&pong);
////    puts("OK");
////
////    return 0;
////}

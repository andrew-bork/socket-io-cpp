#pragma once 

#include <memory>
#include <string_view>
#include <net/socket.hpp>
#include <net/server.hpp>

namespace net {
    net::server create_server(const char * path);
    net::server create_server(int port);


    net::socket connect(const char * address, const char * port);
    net::socket connect(const char * address, int port);
    net::socket connect(const char * path);
};
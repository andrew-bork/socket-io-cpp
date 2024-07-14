#pragma once 

#include <memory>
#include <string_view>
#include <net/socket.hpp>
#include <net/server.hpp>

namespace net {
    net::server create_server(const char * path);
    net::server create_server(int port);


    net::socket connect(std::string address, std::string port);
    net::socket connect(std::string address, int port);
    net::socket connect(std::string path);
};
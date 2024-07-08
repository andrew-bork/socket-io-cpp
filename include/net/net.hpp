#pragma once 

#include <memory>
#include <string_view>
#include <net/socket.hpp>
#include <net/server.hpp>

namespace net {
    std::unique_ptr<server> create_server(const char * path);
    std::unique_ptr<server> create_server(int port);


    std::unique_ptr<socket> connect(const char * address, const char * port);
    std::unique_ptr<socket> connect(const char * address, int port);
    std::unique_ptr<socket> connect(const char * path);
};
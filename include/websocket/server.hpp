#pragma once
#include <string>
#include <list>
#include <functional>
#include "websocket/enums.hpp"
#include "websocket/socket.hpp"

namespace websocket {
    struct server {
        typedef std::function<void(websocket::socket&)> on_connect_handler;
        struct {
            callback_list<on_connect_handler> on_connect;
        } handlers;
        
        void close();
    };
};
#pragma once

#include <vector>
#include <functional>
#include <string>

namespace net {
    struct socket {
        int fd = -1;
        int i = -1;

        enum events {
            CONNECT,
            DATA,
            DISCONNECT,
        };

        socket(int _fd);
        socket(const socket& other);
        socket(socket&& other);
        ~socket();

        

        std::vector<std::function<void()>> on_connect_handlers;
        std::vector<std::function<void(std::string)>> on_data_handlers;
        std::vector<std::function<void()>> on_disconnect_handlers;

        net::socket& on(events event, std::function<void()> handler);
        net::socket& on(events event, std::function<void(std::string)> handler);

        void close();

        size_t operator>>(std::string& string);
        size_t operator<<(const std::string string);
    
        net::socket& operator=(net::socket&&);
        net::socket& operator=(const net::socket&);
    };
};
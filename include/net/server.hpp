#pragma once

#include <vector>
#include <functional>
#include <net/socket.hpp>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <netinet/in.h>

namespace net {
    struct server {
        
        typedef std::function<void(net::socket&)> on_connect_handler;

        socket _socket;

        bool listening = false;
        std::thread * thread;
        // std::unordered_map<int, int> fd_to_socket; // fd -> index -> socket* 
        // server(int fd);

        enum events {
            CONNECT,
            LISTEN,
        };

        std::vector<on_connect_handler> on_connect_handlers;
        std::vector<std::function<void()>> on_listen_handlers;

        server(int fd);
        ~server();

        void listen_threaded(int backlog = 16);
        void listen_block(int backlog = 16);

        net::server& on(events event, std::function<void(net::socket&)> handler);
        net::server& on(events event, std::function<void()> handler);

        int& fd();
    };
};
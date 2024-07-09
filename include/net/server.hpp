#pragma once

#include <vector>
#include <functional>
#include <net/socket.hpp>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <netinet/in.h>
#include <thread> 
namespace net {
    struct server {
        
        typedef std::function<void(net::socket&)> on_connect_handler;
        typedef std::function<void(void)> on_listen_handler;

        socket _socket;

        bool listening = false;
        // std::unordered_map<int, int> fd_to_socket; // fd -> index -> socket* 
        // server(int fd);

        enum events {
            CONNECT,
            LISTEN,
        };

        struct {
            std::vector<on_connect_handler> on_connect;
            std::vector<on_listen_handler> on_listen;
        } handlers;

        std::vector<s

        server(int fd);
        ~server();

        void listen(int backlog = 16);

        net::server& on(events event, std::function<void(net::socket&)> handler);
        net::server& on(events event, std::function<void()> handler);

        int& fd();

        // For event Loops
        void process();
    };
};
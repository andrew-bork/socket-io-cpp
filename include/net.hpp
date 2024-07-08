#ifndef SOCKET_DEFINED
#define SOCKET_DEFINED

#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <arpa/inet.h>
#include <netdb.h>

#include <memory>

#include <string>

#include <poll.h>
#include <thread>
#include <unordered_map>
#include <functional>

// #include <iostream>

#define KB 1024
#define MB 1024*1024

#define chk_bit(a,b) ((a&b) == b)

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
        ~socket();

        
        std::vector<std::function<void()>> on_connect_handlers;
        std::vector<std::function<void(std::string)>> on_data_handlers;
        std::vector<std::function<void()>> on_disconnect_handlers;

        net::socket& on(events event, std::function<void()> handler);
        net::socket& on(events event, std::function<void(std::string)> handler);

        void close();

        size_t operator>>(std::string& string);
        size_t operator<<(const std::string string);
    };

    struct server {
        socket _socket;

        bool listening = false;
        std::thread * thread;
        // std::unordered_map<int, int> fd_to_socket; // fd -> index -> socket* 
        // server(int fd);

        enum events {
            CONNECT,
            LISTEN,
        };

        std::vector<std::function<void(net::socket&)>> on_connect_handlers;
        std::vector<std::function<void()>> on_listen_handlers;

        server(int fd);
        ~server();

        void listen_threaded(int backlog = 16);
        void listen_block(int backlog = 16);

        net::server& on(events event, std::function<void(net::socket&)> handler);
        net::server& on(events event, std::function<void()> handler);

        int& fd();
    };


    std::unique_ptr<server> create_server(const char * path);
    std::unique_ptr<server> create_server(int port);


    std::unique_ptr<socket> connect(const char * address, const char * port);
    std::unique_ptr<socket> connect(const char * address, int port);
    std::unique_ptr<socket> connect(const char * path);

};

#endif
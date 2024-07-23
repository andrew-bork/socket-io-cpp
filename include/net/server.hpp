#pragma once

#include <vector>
#include <functional>
#include <net/socket.hpp>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <netinet/in.h>
#include <thread> 
#include <list>

#include "net/socket.hpp"

namespace net {
    struct server : public net::watchable {
        private:
            net::socket _socket;
            std::list<net::socket> _connections;

            ev_io _read_watcher;

            void _attach();
            void _detach();

            void _initialize_watchers();
            void _destroy_watchers();


        public: 
            typedef std::function<void(net::socket&)> on_connect_handler;
            typedef std::function<void(void)> on_listen_handler;
            
            bool listening = false;

        // std::unordered_map<int, int> fd_to_socket; // fd -> index -> socket* 
        // server(int fd);

        enum events {
            CONNECT,
            LISTEN,
        };

        struct {
            callback_list<on_connect_handler> on_connect;
            callback_list<on_listen_handler> on_listen;
        } handlers;

        // int _fd;
        server(int fd);
        server(const server& other);
        server(server&& other);
        ~server();

        void listen(int backlog = 16);

        net::server& on(events event, on_connect_handler handler);
        net::server& on(events event, on_listen_handler handler);
        inline callback_list<on_connect_handler>::callback_manager on_connect(on_connect_handler handler) {
            return handlers.on_connect.add(handler);
        }
        inline callback_list<on_listen_handler>::callback_manager on_listen(on_listen_handler handler) {
            return handlers.on_listen.add(handler);
        }

        net::server& operator=(const net::server&);
        net::server& operator=(net::server&&);

        int& fd();

    };
};
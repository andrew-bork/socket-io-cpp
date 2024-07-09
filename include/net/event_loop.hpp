#pragma once

#include "net/server.hpp"
#include "net/socket.hpp"
#include <memory>
#include <chrono>
#include <vector>

#include "ev.h"

namespace net {
    struct event_loop {

        event_loop();
        
        void add(std::shared_ptr<net::socket> socket);
        void add(std::shared_ptr<net::server> server);

        void run();
        void run(std::chrono::duration<std::milli> timeout);

        private: 
        
            struct socket_watcher {
                std::shared_ptr<net::socket> socket;
                struct ev_loop* loop;
                ev_io read_watcher, write_watcher;


                socket_watcher(struct ev_loop* _loop, std::shared_ptr<net::socket> _socket);
                ~socket_watcher();

                static void on_readable(EV_P_ ev_io*w, int revents);
                static void on_writable(EV_P_ ev_io*w, int revents);
            };

            struct server_watcher {
                std::shared_ptr<net::server> server;
                struct ev_loop* loop;
                ev_io read_watcher, ;


                server_watcher(struct ev_loop* _loop, std::shared_ptr<net::server> _server);
                ~server_watcher();

                static void on_readable(EV_P_ ev_io*w, int revents);
            }

            struct ev_loop* _loop;
            std::vector<socket_watcher> _watchers;

    };
};
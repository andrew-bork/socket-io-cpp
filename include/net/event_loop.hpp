#pragma once

#include "net/server.hpp"
#include "net/socket.hpp"
#include <memory>
#include <chrono>
#include <vector>

#include "ev++.h"

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
                ev::io read_watcher;

                socket_watcher(ev::loop_ref loop, std::shared_ptr<net::socket> _socket);

                void on_connect(ev::io &w, int revents);
                void on_data(ev::io &w, int revents);
                void on_disconnect(ev::io &w, int revents);
            };

            ev::loop_ref _loop;
            std::vector<socket_watcher> _watchers;
    };
};
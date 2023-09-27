#ifndef HTTP_SERVER_DEFINED
#define HTTP_SERVER_DEFINED

#include <functional>
#include <memory>
#include <iostream>

#include "socket.hpp"
#include "http.hpp"
#include "http_parser.hpp"

namespace http {
    typedef std::function<void(http::request&, http::response&)> route_handler;

    struct server {
        std::shared_ptr<net::server> server_ptr;

        std::unordered_map<std::string, route_handler> path_handler_table;
        route_handler no_match_handler = [](http::request& _, http::response& __){};
        server(std::shared_ptr<net::server> _server_ptr);
        server(int port=80);

        void use(std::string matched_path, route_handler handler);
        void use_default(route_handler handler);

        void listen();

        private:
            void setup_callbacks();
    };
}


void http::server::use(std::string matched_path, route_handler handler) {
    path_handler_table[matched_path] = handler;
}

void http::server::use_default(route_handler handler) {
    no_match_handler = handler;
}

static void debug_print(const std::string& s) {
    std::cout << "\x1b[34m";
    for(size_t i = 0; i < s.length(); i ++) {
        if(s[i] == '\n') std::cout << "\x1b[31m" << "\\n\n" << "\x1b[34m";
        else if(s[i] == '\r') std::cout << "\x1b[31m" << "\\r" << "\x1b[34m";
        else std::cout << s[i];
    }
    std::cout << "\x1b[0m";
}

void http::server::setup_callbacks() {
    this->server_ptr->on(net::server::CONNECT, [&](net::socket& client) {
    
        // std::cout << "client connected" << std::endl;

        http::request_parser req_parser;
        http::response res;

        bool keep_alive = false;

        client.on(net::socket::DATA, [&](std::string chk) {
            // std::cout << <<  << std::endl;
            debug_print(chk);


            req_parser.parse(chk);
            if(req_parser.is_done()) {
                std::cout << "------- (DONE) ----------" << std::endl;
                keep_alive = req_parser.request.headers["connection"] == "keep-alive";

                this->path_handler_table[req_parser.request.url.path.value_or("/")] (req_parser.request, res);
    
            }
        });
        // client.on(net::socket::DISCONNECT, [&] () {
        //     // if(req_parser.is_done());
            

        // });
    });
}

http::server::server(std::shared_ptr<net::server> _server_ptr) : server_ptr(_server_ptr) {
    setup_callbacks();
}

http::server::server(int port) : server_ptr(net::create_server(port)) {
    setup_callbacks();
}

void http::server::listen() {
    server_ptr->listen_block();
}

#endif
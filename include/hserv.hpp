#pragma once

#include <functional>
#include <socket.hpp>



namespace http {
    typedef std::function<void(http::request&, http::response&)> route_handler;

    struct server {
        std::unique_ptr<net::server> server_ptr;

        std::unordered_map<std::string, route_handler> path_handler_table;
        route_handler no_match_handler = [](http::request& _, http::response& __){};
        server(int port=80);

        void use(std::string matched_path, route_handler handler);
        void use_default(route_handler handler);

        void listen();
    };
}


void http::server::use(std::string matched_path, route_handler handler) {
    path_handler_table[matched_path] = handler;
}

void http::server::use_default(route_handler handler) {
    no_match_handler = handler;
}

http::server::server(int port) : server_ptr(net::create_server(port)) {
    this->server_ptr->on(net::server::CONNECT, [&](net::socket& client) {
    
        std::cout << "client connected" << std::endl;

        http::request_parser req_parser;
        http::response res;

        client.on(net::socket::DATA, [&](std::string chk) {
            req_parser.parse(chk);
            std::cout << chk;
            if(req_parser.is_metadata_done()) {
                
                this->path_handler_table[req_parser.request.url.path.value_or("/")] (req_parser.request, res);
            }
        });
        // client.on(net::socket::DISCONNECT, [&] () {
        //     // if(req_parser.is_done());
            

        // });
    });
}

void http::server::listen() {
    server_ptr->listen_block();
}
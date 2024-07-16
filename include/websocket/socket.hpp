#pragma once
#include <string>
#include <list>
#include <functional>
#include "websocket/enums.hpp"
#include "net/socket.hpp"

namespace websocket {
    struct socket {
        struct {
            std::list<std::function<void(void)>> on_connect;
            std::list<std::function<void(const std::string&)>> on_message;
            std::list<std::function<void(void)>> on_disconnect;
        } handlers;

        net::socket& _socket;

        socket(net::socket& socket);

        
        void send(const std::string& data);



        void close();




        template<websocket::events T, typename F>
        void on(F f) {
            
        }

        template<>
        void on<websocket::CONNECT>(std::function<void(void)> f);

        template<>
        void on<websocket::MESSAGE>(std::function<void(const std::string&)> f);

        template<>
        void on<websocket::DISCONNECT>(std::function<void(void)> f);

    };
};
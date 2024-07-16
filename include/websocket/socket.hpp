#pragma once
#include <string>
#include <list>
#include <functional>
#include "websocket/enums.hpp"
#include "net/socket.hpp"
#include "url/url.hpp"
namespace websocket {
    struct socket {


        url::url _url;
        net::socket _socket;

        socket(const net::socket& socket, std::string path="/");
        socket(net::socket&& socket, std::string path="/");
        socket(std::string url);
        socket(url::url _url);

        

        void send(const std::string& data);

        void initiate_handshake();

        void close();



        struct {
            std::list<std::function<void(void)>> on_open;
            std::list<std::function<void(void)>> on_close;
            std::list<std::function<void(std::string)>> on_message;
        } handlers;

        void on_open(std::function<void(void)> f);
        void on_close(std::function<void(void)> f);
        void on_message(std::function<void(std::string)> f);

    };
};
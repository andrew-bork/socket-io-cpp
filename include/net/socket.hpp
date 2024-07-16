#pragma once

#include <vector>
#include <functional>
#include <string>

#include <list>

namespace net {
    struct socket {
        int fd = -1;
        int i = -1;

        bool connected = false;
        bool opened = false;

        enum events {
            CONNECT,
            DATA,
            DISCONNECT,
        };

        socket(int _fd);
        socket(const socket& other);
        socket(socket&& other);
        ~socket();

        
        struct {
            std::list<std::function<void()>> on_connect;
            std::list<std::function<void(std::string)>> on_data;
            std::list<std::function<void()>> on_disconnect;
        } handlers;


        // template<events E, typename F>
        // void on(F f) {}

        // template<>
        // void on<events::CONNECT>(std::function<void()> f);
        // template<>
        // void on<events::DATA>(std::function<void(std::string)> f);
        // template<>
        // void on<events::DISCONNECT>(std::function<void()> f);

        void on_connect(std::function<void(void)> f);
        void on_data(std::function<void(std::string)> f);
        void on_disconnect(std::function<void(void)> f);

        net::socket& on(events event, std::function<void()> handler);
        net::socket& on(events event, std::function<void(std::string)> handler);

        void close();

        size_t operator>>(std::string& string);
        size_t operator<<(const std::string string);
    
        net::socket& operator=(net::socket&&);
        net::socket& operator=(const net::socket&);
    };
};
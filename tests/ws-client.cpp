#include <iostream>

#include "websocket/socket.hpp"

int main() {
    net::socket sock(-1);
    websocket::socket s(sock);
    s.on<websocket::CONNECT>([](){
        std::cout << "Connected!\n";
    });
    
}
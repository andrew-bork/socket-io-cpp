#include <iostream>

#include "websocket/socket.hpp"

int main() {
    net::socket sock(-1);
    websocket::socket s(sock);
    s.on_open([](){
        std::cout << "Connected!\n";
    });
    
}
#include <iostream>

#include "websocket/socket.hpp"
#include "net/net.hpp"
#include "net/event_loop.hpp"
int main() {
    net::socket sock = net::connect("localhost", 1234);
    // sock << /



    
    websocket::socket s(sock);
    s.on_open([](){
        std::cout << "Connected!\n";
    });
    
    net::event_loop e;
    e.add(s.connection());
    // sock.on_disconnect([]() {
    //     std::cout << "Thats rude\n";
    // });
    s.set_host("localhost:1234");
    s.initiate_client_handshake();


    e.run();

}
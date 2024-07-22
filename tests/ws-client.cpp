#include <iostream>

#include "websocket/socket.hpp"
#include "net/net.hpp"
#include "net/event_loop.hpp"
int main() {
    net::socket sock = net::connect("localhost", 1234);
    // sock << /



    
    websocket::socket s(sock);
    s.on_open([&](){
        std::cout << "Connected!\n";
        s.send("Hello world!");
    });
    
    net::event_loop e;
    e.add(s.connection());
    // s.connection().on_data([&] (std::span<const char> data) { 
    //         std::cout << "Client: " << std::hex << "\n"; // << std::string(data.begin(), data.end()) << std::endl;
    //         for(char c : data) {
    //             std::cout << "\t" << (int) (unsigned char) c;
    //             if(isprint(c)) {
    //                 std::cout << " (" << c << ")";
    //             }
    //             std::cout << std::endl;
    //         }
    //         // socket << "Pong!";
    //     });
    // sock.on_disconnect([]() {
    //     std::cout << "Thats rude\n";
    // });
    s.set_host("localhost:1234");
    s.initiate_client_handshake();

    // s.send("Hello world!");

    e.run();

}
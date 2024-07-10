#include "net/net.hpp"
#include "net/event_loop.hpp"

#include <poll.h>

#define chk_flag(a,b) ((a & b) == b)

#include <vector>
#include <csignal>
#include <thread>
#include <iostream>

// bool running = true;

net::server server(-1);

void interrupt_handler(int code) {
    server.listening = false;
}



int main() {
    
    net::event_loop ev;
    server = net::create_server(3000);

    server.on(net::server::LISTEN, []() {
        printf("Server listening on \"localhost:3000\".\n");
    });

    server.on(net::server::CONNECT, [&](net::socket& socket) {
        printf("Client Connected\n");
        
        ev.add(socket);
        socket.on(net::socket::events::DATA, [&] (std::string data) { 
            std::cout << "Client: " << data << std::endl;
            socket << "Pong!";
        });

        socket.on(net::socket::events::DISCONNECT, [] () {
            std::cout << "Goodbye\n";
        });
    });


    server.listen();

    ev.add(server);

    ev.run();

    printf("Exited Gracefully\n");
}   
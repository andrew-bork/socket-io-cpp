#include "net/net.hpp"
#include "net/event_loop.hpp"

#include <poll.h>

#define chk_flag(a,b) ((a & b) == b)

#include <vector>
#include <csignal>
#include <thread>
#include <iostream>
#include <list>

// bool running = true;

net::server server(-1);

void interrupt_handler(int code) {
    server.listening = false;
    ev_loop_destroy(server.loop()->_loop);
    
}


int main(int argc, char ** argv) {
    
    
    short port = 3000;
    if(argc >= 2) {
        port = atoi(argv[1]);
    }


    net::event_loop ev;
    server = net::create_server(port);

    server.on(net::server::LISTEN, []() {
        printf("Server listening on \"localhost:3000\".\n");
    });
    
    std::list<net::socket*> connections;

    server.on(net::server::CONNECT, [&](net::socket& socket) {
        printf("Client Connected\n");
        auto i = connections.end();
        connections.emplace_back(&socket);
        ev.add(socket);
        socket.on_data([&] (std::span<const char> data) { 
            std::cout << "Client: " << std::hex << "\n"; // << std::string(data.begin(), data.end()) << std::endl;
            for(char c : data) {
                std::cout << "\t" << (int) (unsigned char) c;
                if(isprint(c)) {
                    std::cout << " (" << c << ")";
                }
                std::cout << std::endl;
            }
            for(auto k : connections) {
                k->write(data);
                k->drain();
            }
        });
        // socket << 

        socket.on_disconnect([&] () {
            std::cout << "Goodbye\n";
            connections.erase(i);
        });
    });

    std::cout<<"huh\n";

    server.listen();

    ev.add(server);

    ev.run();

    printf("Exited Gracefully\n");
}   
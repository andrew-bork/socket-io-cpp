#include <socket.hpp>
#include <poll.h>

#define chk_flag(a,b) ((a & b) == b)

#include <vector>
#include <csignal>
#include <thread>
#include <iostream>

// bool running = true;

std::unique_ptr<net::server> server;

void interrupt_handler(int code) {
    server->listening = false;
}



int main() {
    server = net::create_server(3000);

    server->on(net::server::LISTEN, []() {
        printf("Server listening on \"localhost:3000\".\n");
    });

    server->on(net::server::CONNECT, [](net::socket& socket) {
        printf("Client Connected\n");

        socket.on(net::socket::events::DATA, [&] (std::string data) { 
            std::cout << "Client: " << data << std::endl;
            socket << "Pong!";
        });
    });

    server->listen_block();

    printf("Exitted Gracefully\n");
}   
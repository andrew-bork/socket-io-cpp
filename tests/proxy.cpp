#include "net/net.hpp"
#include "net/event_loop.hpp"

#include <poll.h>

#define chk_flag(a,b) ((a & b) == b)

#include <vector>
#include <csignal>
#include <thread>
#include <iostream>


#include "url/url.hpp"
// bool running = true;

// net::server server(-1);

void interrupt_handler(int code) {
    // server.listening = false;
}



int main(int argc, char** argv) {
    
    short port = 3000;
    url::url parsed_url;

    if(argc < 3) {
        printf("proxy [PORT] [URL]\nHost a server on a port which connects to a url.\n")l
        return -1;
    }
    parsed_url = url::parse(argv[2]);
    port = atoi(argv[1]);


    net::event_loop ev;
    
    net::server proxy_server = net::create_server(port);
    proxy_server.on(net::server::LISTEN, []() {
        printf("Server listening on \"localhost:%d\".\n");
    });

    std::list<net::socket> server_conns;

    proxy_server.on(net::server::CONNECT, [&](net::socket& client) {
        printf("Client Connected\n");
        server_conns.push_back(net::connect(parsed_url.host.value().c_str(), parsed_url.port.value_or(80)));
        auto server_conn_iter = --server_conns.end();

        
        // socket.on_data([&] (std::span<const char> data) { 
        //     std::cout << "Client: " << std::hex << "\n"; // << std::string(data.begin(), data.end()) << std::endl;
        //     for(char c : data) {
        //         std::cout << "\t" << (int) (unsigned char) c;
        //         if(isprint(c)) {
        //             std::cout << " (" << c << ")";
        //         }
        //         std::cout << std::endl;
        //     }
        //     socket << "Pong!";
        // });
        // socket << 

        // socket.on_disconnect([] () {
        //     std::cout << "Goodbye\n";
        // });

        ev.add(*server_conn_iter);
    });

    proxy_server.listen();

    // ev.add(proxy_server);\

    ev.run();

    printf("Exited Gracefully\n");
}   
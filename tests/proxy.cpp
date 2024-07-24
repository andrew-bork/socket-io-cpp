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
    
    short port = 5001;
    url::url parsed_url = url::parse("http://localhost:3000");

    // if(argc < 3) {
    //     printf("proxy [PORT] [URL]\nHost a server on a port which connects to a url.\n");
    //     return -1;
    // }
    // parsed_url = url::parse(argv[2]);
    // port = atoi(argv[1]);


    net::event_loop ev;
    
    net::server proxy_server = net::create_server(port);
    proxy_server.on(net::server::LISTEN, [=]() {
        printf("Server listening on \"localhost:%d\".\n", port);
    });

    std::list<net::socket> server_conns;

    proxy_server.on(net::server::CONNECT, [&](net::socket& client) {
        printf("Client Connected\n");
        std::string p = parsed_url.port.value_or("80");
        auto& server_conn = server_conns.emplace_back(net::connect(parsed_url.host.value().c_str(), p.c_str()));
        auto server_conn_iter = --server_conns.end();

        client.on_data([&](std::span<const char> data) {
            server_conn << std::string(data.begin(), data.end());
        });

        server_conn.on_data([&](std::span<const char> data) {
            client << std::string(data.begin(), data.end());
        });

        client.on_disconnect([&]() {
            server_conn.close();
            server_conns.erase(server_conn_iter);
        });

        server_conn.on_disconnect([&]() {
            client.close();
            server_conns.erase(server_conn_iter);
        });

        ev.add(server_conn);
        ev.add(client);
    });

    proxy_server.listen();

    ev.add(proxy_server);

    ev.run();

    printf("Exited Gracefully\n");
}   
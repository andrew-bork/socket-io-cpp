#include <net/net.hpp>
#include <net/event_loop.hpp>

#include <list>
#include <iostream>

int main(int argc, char ** argv) {
    
    short port = 3000;
    if(argc >= 2) {
        port = atoi(argv[1]);
    }
    // std::vector<std::shared_ptr<net::socket>> conns;

    // net::event_loop ev;
    // printf("POSIX:%d\n", __POSIX_VISIBLE);

    int n_connections = 1;
    
    net::event_loop loop;

    std::list<net::socket> connections;

    for(int i = 0; i < n_connections; i ++) {
        auto& a = connections.emplace_back(net::connect("localhost", port));
        auto iter = --connections.end();
        a.on_data([&] (std::span<const char> s) {
            std::cout << "Server: " << std::string(s.begin(), s.end()) << std::endl;
            a.close();
        });
        a.on_disconnect([&]() {
            connections.erase(iter);
            std::cout << "welp\n";
        });
        a << "GET / HTTP/1.0\r\n\r\n";
        // ev.add(a);
        loop.add(a);
    }
    std::cout << "Starting Loop" << std::endl;
    loop.run();
    // while(true);
    // ev.start();
    // auto socket = net::connect("www.google.com", "80");
    // std::string s;
    // (*socket) >> s;
    // std::cout << s << std::endl;
}   
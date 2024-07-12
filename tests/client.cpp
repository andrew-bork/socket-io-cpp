#include <net/net.hpp>
#include <net/event_loop.hpp>

#include <list>
#include <iostream>

int main(int argc, char ** argv) {
    
    int n_connections = 1;
    if(argc >= 2) {
        n_connections = atoi(argv[1]);
    }
    // std::vector<std::shared_ptr<net::socket>> conns;

    // net::event_loop ev;
    // printf("POSIX:%d\n", __POSIX_VISIBLE);


    
    net::event_loop loop;

    std::list<net::socket> connections;

    for(int i = 0; i < n_connections; i ++) {
        auto& a = connections.emplace_back(net::connect("localhost", "3003"));
        a.on(net::socket::events::DATA, [&] (std::string s) {
            std::cout << "Server: " << s << std::endl;
            a.close();
        });
        a.on(net::socket::DISCONNECT, []() {
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
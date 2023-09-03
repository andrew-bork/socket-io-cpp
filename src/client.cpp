#include <socket.hpp>
#include <iostream>

int main(int argc, char ** argv) {
    
    int n_connections = 1;
    if(argc >= 2) {
        n_connections = atoi(argv[1]);
    }
    // std::vector<std::shared_ptr<net::socket>> conns;

    net::event_loop ev;

    for(int i = 0; i < n_connections; i ++) {
        std::shared_ptr<net::socket> a = net::connect("www.google.com", "80");
        a->on(net::socket::events::DATA, [] (std::string s) {
            std::cout << "Server: " << s << std::endl;
        });
        a->on(net::socket::DISCONNECT, []() {
            std::cout << "welp\n";
        });
        (*a) << "GET / HTTP/1.0\r\n\r\n";
        ev.add(a);
    }

    ev.start();
    // auto socket = net::connect("www.google.com", "80");
    // std::string s;
    // (*socket) >> s;
    // std::cout << s << std::endl;
}   
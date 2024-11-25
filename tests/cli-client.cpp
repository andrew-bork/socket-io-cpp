#include <iostream>
#include <net/net.hpp>
#include <net/event_loop.hpp>

#include <list>
#include <iostream>

#include <thread>
#include <string>

net::socket* k;

void input_loop() {
    while(true) {
        std::string inp;
        std::getline(std::cin, inp);
        k->write(std::string_view(inp));
    }
}

int main(int argc, char ** argv) {
    
    short port = 3000;
    if(argc >= 2) {
        port = atoi(argv[1]);
    }

    std::thread th(input_loop);
    // std::vector<std::shared_ptr<net::socket>> conns;

    // net::event_loop ev;
    // printf("POSIX:%d\n", __POSIX_VISIBLE);
    net::socket s = net::connect("localhost", port);
    k = &s;
    
    net::event_loop loop;

    s.on_data([&] (std::span<const char> s) {
        std::cout << "Server: " << std::string(s.begin(), s.end()) << std::endl;
    });

    s.on_disconnect([&]() {
        std::cout << "welp\n";
    });

    // s << "GET / HTTP/1.0\r\n\r\n";
    // ev.add(a);
    loop.add(s);

    std::cout << "Starting Loop" << std::endl;
    loop.run();
    // while(true);
    // ev.start();
    // auto socket = net::connect("www.google.com", "80");
    // std::string s;
    // (*socket) >> s;
    // std::cout << s << std::endl;
}   

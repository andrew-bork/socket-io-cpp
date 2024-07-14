#include <net/net.hpp>
#include <net/event_loop.hpp>
#include "http/request.hpp"
#include <list>
#include <iostream>
#include "http/parser.hpp"

void debug_print(std::string s) {
    for(auto i : s) {
        if(i == '\r') {
            std::cout << "\u001b[35m\\r\u001b[0m";
        }else if(i == '\n') {
            std::cout << "\u001b[35m\\n\u001b[0m\n";
        }else {

            std::cout << i;
        }
    }
}

int main(int argc, char ** argv) {
    
    std::string url = "http://google.com";
    if(argc >= 2) {
        url = std::string(argv[1]);
    }
    // std::vector<std::shared_ptr<net::socket>> conns;

    // net::event_loop ev;
    // printf("POSIX:%d\n", __POSIX_VISIBLE);


    http::request req;
    req.method = http::request::GET;
    req.url = url::parse("http://www.example.com");
    req.headers["host"] = req.url.domain.value();
    // req.headers["connection"] = "close";
    
    net::event_loop loop;
    http::response_parser parser;
    parser.body.on(http::stream::DATA, [&](std::string data) {
        std::cout << data;
        // std::cout << "Body: " << data.length() << " bytes\n";

    });
    parser.body.on(http::stream::END, [&]() {
        // std::cout << "Body Finished\n";
    });

    net::socket a = net::connect(req.url.domain.value(), "80");
    a.on(net::socket::events::DATA, [&] (std::string s) {
        // std::cout << "\nServer: " << s.length() << " bytes" << std::endl;
        // debug_print(s.substr(0, 50));
        // debug_print(s.substr(s.length()-50));
        // std::cout << "\n";
        if(parser.parse(s)) a.close();
    });
    a.on(net::socket::DISCONNECT, []() {
        // std::cout << "welp\n";
    });
    loop.add(a);
    a << req.build();
    loop.run();

    // std::cout << "huh\n";
}   

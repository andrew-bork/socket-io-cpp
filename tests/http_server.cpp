#include "http.hpp"
#include "hserv.hpp"
#include <iostream>

int main() {
    http::server server;
    server.use("/", [] (http::request& a, http::response& b) {
        std::cout << "PP POO POO" << std::endl;
    });
    server.server_ptr->on(net::server::LISTEN, []() {
        std::cout << "Listening" << std::endl;
    });
    server.listen();
}
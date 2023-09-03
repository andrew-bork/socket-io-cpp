#include <http.hpp>
#include <iostream>

int main() {
    http::server server;
    server.use("/", [] (http::request& a, http::response& b) {
        std::cout << "PP POO POO" << std::endl;
    });
    server.listen();
}
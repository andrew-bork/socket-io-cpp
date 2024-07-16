#include "websocket/socket.hpp"
#include "net/net.hpp"
#include <stdexcept>
websocket::socket::socket(const net::socket& socket, std::string path) : _socket(socket) {
    _url.path = path;
}

websocket::socket::socket(net::socket&& socket, std::string path) : _socket(socket) {
    _url.path = path;
}

websocket::socket::socket(std::string url) : socket(url::parse(url)) {
    
}

websocket::socket::socket(url::url _url) : _socket(-1) {
    if(_url.host.has_value()) throw std::runtime_error("no host");
    if(_url.protocol.value_or("ws") == "ws") throw std::runtime_error("Wrong Protocol");
    _socket = net::connect(_url.host.value(), _url.port.value_or("80"));
}


void websocket::socket::initiate_handshake() {
    // _socket.on_connect(); 
}

void websocket::socket::send(const std::string& data) {
    // socket << data;
}

void websocket::socket::close() {
    
}

void websocket::socket::on_open(std::function<void(void)> f) {
    handlers.on_open.push_back(f);
}
void websocket::socket::on_close(std::function<void(void)> f) {
    handlers.on_close.push_back(f);
}
void websocket::socket::on_message(std::function<void(std::string)> f) {
    handlers.on_message.push_back(f);
}

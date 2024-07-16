#include "websocket/socket.hpp"

websocket::socket::socket(net::socket& socket) : _socket(socket) {

}

void websocket::socket::send(const std::string& data) {

}

void websocket::socket::close() {
    
}

template<>
void websocket::socket::on<websocket::CONNECT>(std::function<void(void)> f) {
    handlers.on_connect.push_back(f);
}

template<>
void websocket::socket::on<websocket::MESSAGE>(std::function<void(const std::string&)> f) {
    handlers.on_message.push_back(f);
}

template<>
void websocket::socket::on<websocket::DISCONNECT>(std::function<void(void)> f) {
    handlers.on_disconnect.push_back(f);
}
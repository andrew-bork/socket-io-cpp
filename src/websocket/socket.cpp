#include "websocket/socket.hpp"
#include "net/net.hpp"
#include <stdexcept>

#include "http/request.hpp"
#include <random>
#include <climits>
#include <algorithm>
#include <openssl/crypto.h>
#include "base64.hpp"

#include "util/hash.hpp"

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


using random_bytes_engine = std::independent_bits_engine<std::default_random_engine, CHAR_BIT, unsigned char>;
static std::string generate_websocket_key() {
    random_bytes_engine rbe;
    // rbe.seed()
    std::string key(16, '\0');
    std::generate(key.begin(), key.end(), std::ref(rbe));
    return key;
}



#include <iostream>

net::socket& websocket::socket::connection() {
    return _socket;
}

void websocket::socket::initiate_client_handshake() {
    _handshake = new websocket::socket::handshake_manager(*this);
}

void websocket::socket::send(const std::string& data) {
    // socket << data;
}

void websocket::socket::close() {
    
}

// void websocket::socket::on_open(std::function<void(void)> f) {
//     handlers.on_open.push_back(f);
// }
// void websocket::socket::on_close(std::function<void(void)> f) {
//     handlers.on_close.push_back(f);
// }
// void websocket::socket::on_message(std::function<void(std::string)> f) {
//     handlers.on_message.push_back(f);
// }

void websocket::socket::set_host(std::string host) {
    _url.host = host;
}

websocket::socket::~socket() {
    if(_handshake != NULL) delete _handshake;
}

// static void debug_print(std::string s) {
//     for(auto i : s) {
//         if(i == '\r') {
//             std::cout << "\u001b[35m\\r\u001b[0m";
//         }else if(i == '\n') {
//             std::cout << "\u001b[35m\\n\u001b[0m\n";
//         }else {

//             std::cout << i;
//         }
//     }
// }


websocket::socket::handshake_manager::handshake_manager(websocket::socket& socket) : _socket(socket) {

    _response_parser.reset();
    _sock_data_handler =  socket._socket.on_data([&](std::span<const char> data) {
        // std::cout << data;
        std::string s = std::string(data.begin(), data.end());
        std::clog << std::endl << "\e[0;34m" << s << "\e[0m" << std::endl;
        // debug_print(s);
        if(_response_parser.parse(s)) {
            std::cout << '\t'<< socket._handshake->_secure_key_accept << "\n\t" << _response_parser.response.headers["sec-websocket-accept"] << "\n";
            if(socket._handshake->_secure_key_accept != _response_parser.response.headers["sec-websocket-accept"])
                throw std::runtime_error("Handshake Failed, sec-websocket-key mismatch.");
            _sock_data_handler.remove();
            delete socket._handshake;
            socket._handshake = NULL;
            socket.handlers.on_open.call();
        }
    });

    http::request req;
    req.method = http::request::GET;
    req.url = socket._url;
    req.headers["host"] = socket._url.get_host();
    req.headers["upgrade"] = "websocket";
    req.headers["connection"] = "upgrade";
    
    _secure_key = base64::to_base64(generate_websocket_key());

    req.headers["sec-websocket-key"] = _secure_key;
    req.headers["sec-websocket-version"] = "13";
    // std::cout << req.build() << std::endl << std::endl;

    _socket._socket << req.build();
    
    // _sock_connect_handler = socket._socket.on_connect([&]() {
    //     // _sock_connect_handler.remove();
    // });


    hash::sha1 hasher;
    hasher.hash(_secure_key);
    hasher.hash("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
    _secure_key_accept = base64::to_base64(hasher.get());
}


websocket::socket::handshake_manager::~handshake_manager() {
    // if(_hasher_ctx != NULL) EVP_MD_CTX_free(_hasher_ctx);
}
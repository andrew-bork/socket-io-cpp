#include "http/get.hpp"
#include "net/net.hpp"
#include "url/url.hpp"
#include <stdexcept>

http::response http::get(const std::string& url_string) {
    http::request req;
    req.url = url::parse(url_string);
    req.headers["Connection"] = "close";
    return http::get(req);
}

http::response http::get(http::request req) {
    std::string protocol = req.url.protocol.value_or("http");
    if(protocol == "http") {

    }else if(protocol == "https") {

    }else {
        throw std::runtime_error(protocol + " protocol not supported");
    }

    if(!req.url.host.has_value()) throw std::runtime_error("URL has no domain.");

    // net::event_loop ev;
    auto sock = net::connect(req.url.host.value().c_str(), req.url.port.value_or("80").c_str());
    std::string out = "";
    sock.on(net::socket::DATA,[&](std::string chk) {
        out += chk;
    });
    sock << req.build();
    // ev.add(sock);
    // ev.start();
    return http::response();
}
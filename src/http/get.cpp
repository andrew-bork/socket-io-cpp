#include "http/get.hpp"
#include "net/net.hpp"
#include "http/url.hpp"
#include <stdexcept>

http::response http::get(const std::string& url_string) {
    http::request req;
    auto parsed = url::parse(url_string);
    // if(!parsed.has_value()) throw std::runtime_error("Couldn't parse url.");
    req.url = parsed.value();
    req.headers["Connection"] = "close";
    return http::get(req);
}

http::response http::get(const http::request req) {
    std::string protocol = req.url.protocol.value_or("http");
    if(protocol == "http") {

    }else if(protocol == "https") {

    }else {
        throw std::runtime_error(protocol + " protocol not supported");
    }

    if(!req.url.domain.has_value()) throw std::runtime_error("URL has no domain.");

    // net::event_loop ev;
    auto sock = net::connect(req.url.domain.value().c_str(), req.url.port.value_or("80").c_str());
    std::string out = "";
    sock.on(net::socket::DATA,[&](std::string chk) {
        out += chk;
    });
    sock << req.build();
    // ev.add(sock);
    // ev.start();
    return http::response();
}
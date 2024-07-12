#include "http/parser.hpp"
#include <iostream>
#include "util/string.hpp"
#include <stdexcept>

bool http::request_parser::is_done() {
    // return state == DONE;
    return false;
}

bool http::request_parser::is_header_done() {
    // return state == BODY;
    return false;
}


http::request_parser::request_parser() {
}

http::response_parser::response_parser() {
}

bool http::request_parser::parse(const std::string& str) {
return false;
}



bool http::response_parser::parse(const std::string& str) {
    return false;
}


http::stream& http::stream::on(events event, std::function<void(std::string)> handler) {
    switch(event) {
    case DATA:
        handlers.on_data.push_back(handler);
        for(auto handler : handlers.on_data) {
            handler(unparsed);
        }
        unparsed = "";
        break;
    default:
        throw std::runtime_error("\"CONNECT\" listener has the wrong type");
    }
    return *this;
}
http::stream& http::stream::on(events event, std::function<void()> handler) {
    switch(event) {
    case END:
        handlers.on_end.push_back(handler);
        break;
    default:
        throw std::runtime_error("\"CONNECT\" listener has the wrong type");
    }
    return *this;
}

void http::stream::write(std::string data) {
    if(unparsed.length() + data.length() > 4 * 1024 * 1024) {
        throw std::runtime_error("message too large");
    }
    unparsed += data;
    if(handlers.on_data.empty()) return;
    for(auto handler : handlers.on_data) {
        handler(unparsed);
    }
    unparsed = "";
}
void http::stream::close() {
    for(auto handler : handlers.on_end) {
        handler();
    }
}
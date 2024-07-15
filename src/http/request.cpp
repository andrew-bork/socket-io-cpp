#include "http/request.hpp"

std::string http::request::build() {
    std::string out = "";
    out += http::get_request_type(method);
    out += " ";
    if(url.path.has_value() && url.path.value().size() > 0) {
        out += url.path.value();
    }else {
        out += "/";
    }

    if(!url.queries.empty()) {
        out += "?";

        bool started = false;
        for(auto i = url.queries.begin(); i != url.queries.end(); i ++) {
            if(!started) started = true;
            else out += "&";
            out += (*i).first;
            out += "="; 
            out += (*i).second;
        }
    }

    out += " ";
    out += "HTTP/1.1";
    out += "\r\n";

    if(body.has_value()) headers["content-length"] = std::to_string(body.value().length());

    for(auto i = headers.begin(); i != headers.end(); i++) {
        out += (*i).first;
        out += ":"; 
        out += (*i).second;
        out += "\r\n";
    }


    if(body.has_value())
        out += body.value() + "\r\n";

    out += "\r\n";

    return out;
}


std::string http::get_request_type(http::request::request_type type) {
    switch(type) {
    case http::request::request_type::GET:
        return "GET";
    case http::request::request_type::POST:
        return "POST";
    }
    throw std::runtime_error("Invalid Request Type");
}
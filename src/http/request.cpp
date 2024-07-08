#include "http/request.hpp"

std::string http::request::build() const {
    std::string out = "";
    out += get_request_type(type);
    out += " ";
    out += url.path.value_or("/");

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

    for(auto i = headers.begin(); i != headers.end(); i ++) {
        out += (*i).first;
        out += ":"; 
        out += (*i).second;
        out += "\r\n";
    }

    out += body;

    if(!body.empty())
        out += "\r\n";

    out += "\r\n";

    return out;
}

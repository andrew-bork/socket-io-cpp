#pragma once
#include <string>
#include <unordered_map>
#include <iostream>
#include <queue>

#include <socket.hpp>
#include <url.hpp>
// #include <regex>

namespace http {

    struct request {
        enum request_type {
            GET, POST
        } type = GET;

        url url;

        std::string version = "HTTP/1.1";
        std::unordered_map<std::string, std::string> headers;
        std::string body = "";

        static http::request parse(const std::string& response_string);
        std::string build() const;
    };
    struct response {

        std::string version = "";
        std::string status_reason = "";
        std::string status_code = "";
        std::string body = "";
        std::unordered_map<std::string, std::string> headers;
        

        static response parse(const std::string& response_string);
    };

    struct response_parser {
        response response;

        enum {
            VERSION, STATUS_REASON, STATUS_CODE, HEADER_OR_BODY, HEADER_KEY, HEADER_VALUE_WHITESPACE, HEADER_VALUE, BODY, NONE, FINISHED
        } currently_parsing = VERSION;
        bool last_char_was_cr = false;

        std::string prev_unused_token = "";

        bool parse(const std::string& response_string);
    };

    struct request_parser {
        enum State {
            REQUEST_LINE,
            HEADERS,
            BODY,
            DONE
        } state = REQUEST_LINE;

        http::request request;

        std::string unparsed = "";
        bool is_metadata_done();
        bool is_done();
        bool parse(const std::string& str);
    };

    std::string curl(const std::string url_string);

    std::string curl(const http::request req);
};

static std::string get_request_type(http::request::request_type type) {
    switch(type) {
    case http::request::request_type::GET:
        return "GET";
    case http::request::request_type::POST:
        return "POST";
    }
}

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

std::string http::curl(const std::string url_string) {
    http::request req;
    auto parsed = url::parse(url_string);
    // if(!parsed.has_value()) throw std::runtime_error("Couldn't parse url.");
    req.url = parsed.value();
    req.headers["Connection"] = "close";
    return http::curl(req);
}

std::string http::curl(const http::request req) {
    std::string protocol = req.url.protocol.value_or("http");
    if(protocol == "http") {

    }else if(protocol == "https") {

    }else {
        throw std::runtime_error(protocol + " protocol not supported");
    }

    if(!req.url.domain.has_value()) throw std::runtime_error("URL has no domain.");

    net::event_loop ev;
    std::shared_ptr<net::socket> sock = net::connect(req.url.domain.value().c_str(), req.url.port.value_or("80").c_str());
    std::string out = "";
    sock->on(net::socket::DATA,[&](std::string chk) {
        out += chk;
    });
    *sock << req.build();
    ev.add(sock);
    ev.start();
    return out;
}


static std::string parse_up_to_char(size_t& i, const std::string& response_string, char c) {
    std::string out = "";
    while(i < response_string.size()) {
        if(response_string[i] == c) {
            i++;
            return out;
        }else {
            out += response_string[i];
            i++;
        }
    }
    return out;
}

static std::string parse_up_to_CRLF(size_t& i, const std::string& response_string) {
    std::string out = "";
    while(i+1 < response_string.size()) {
        if(response_string[i] == '\r' && response_string[i+1] == '\n') {
            i+=2;
            return out;
        }else {
            out += response_string[i];
            i++;
        }
    }
    if(i < response_string.size()) {
        out += response_string[i];
        i++;
    }
    return out;
}

static void parse_status_line(size_t& i, const std::string& response_string, http::response& response) {
    response.version = parse_up_to_char(i, response_string, ' ');
    response.status_code = parse_up_to_char(i, response_string, ' ');
    response.status_reason = parse_up_to_CRLF(i, response_string);
}

static http::request::request_type parse_request_type(const std::string str) {
    if(str == "GET") {
        return http::request::GET;
    }else if(str == "POST") {
        return http::request::POST;
    }

    throw std::runtime_error("Unkown method");
}

static void parse_request_line(size_t& i, const std::string& req_str, http::request& req) {
    req.type = parse_request_type(parse_up_to_char(i, req_str, ' '));
    req.url.path = parse_up_to_char(i, req_str, ' ');
    /* req.version = */parse_up_to_CRLF(i, req_str);
}

static void parse_headers(size_t& i, const std::string& str, std::unordered_map<std::string, std::string>& headers) {
    bool parsing_headers = true;
    while(parsing_headers) {
        std::string key = parse_up_to_char(i, str, ':');
        std::string val = parse_up_to_CRLF(i, str);
        headers[key] = val;
    }
}
http::request http::request::parse(const std::string& req_str) {
    size_t i = 0;
    http::request out;
    parse_request_line(i, req_str, out);

    parse_headers(i, req_str, out.headers);
    
    out.body = req_str.substr(i);
    // std::cout << "v: " << out.version << " code: " << out.status_code << " reason: " << out.status_reason << std::endl;

    return out;
}
http::response http::response::parse(const std::string& response_string) {
    size_t i = 0;
    http::response out;
    parse_status_line(i, response_string, out);

    parse_headers(i, response_string, out.headers);
    
    std::cout << "v: " << out.version << " code: " << out.status_code << " reason: " << out.status_reason << std::endl;

    out.body = response_string.substr(i);
    return out;
}

// static std::optional<std::string> next_token(size_t& i, const std::string& response_string) {
//     size_t old_i = i;
//     std::string out = "";
//     while(i < response_string.size()) {
//         char c = response_string[i];
//         i ++;
//         if(c == '\r' && i+1 < response_string.size() && response_string[i+1] == '\n') {
//             return out;
//         }

//         out += c;
//     }

//     i = old_i;
//     return std::nullopt;
// }

bool http::response_parser::parse(const std::string& response_string) {
    // size_t i = 0;
    // auto next_token = next_token_newline(i, response_string)
    

    return false;

}


bool http::request_parser::is_done() {
    return state == DONE;
}

bool http::request_parser::is_metadata_done() {
    return state == BODY || state == DONE;
}


static void parse_request_line(const std::string& str, http::request& out) {
    size_t i = 0;
    size_t j = str.find(' ');
    std::string method = str.substr(i, j - i);
    i = j + 1;
    j = str.find(' ', i);
    std::string path = str.substr(i, j - i);
    i = j + 1;
    std::string version = str.substr(i);

    // std::cout << "Version: " << version << " Path: " << path << " Method: " << method << std::endl;

    out.version = out.version;
    out.type = parse_request_type(method);
    out.url = url::parse_absolute_path(path);
}

bool http::request_parser::parse(const std::string& str) {
    size_t i = 0;
    bool running = true;
    while (running) {
        running = false;
        size_t j;
        
        unparsed += str;
        switch(state) {
        case REQUEST_LINE:
            j = unparsed.find("\r\n");
            if(j != std::string::npos) {
                std::string req_line = unparsed.substr(i, j - i);
                
                parse_request_line(req_line, request);

                unparsed = unparsed.substr(j+2);
                i = 0;
                state = HEADERS;
            }
            break;
        case HEADERS:
            j = unparsed.find("\r\n");
            if(j != std::string::npos) {
                std::string header_line = unparsed.substr(i, j - i);
                if(header_line.empty()) {
                    std::cout << "END OF HEADERS" << std::endl;
                    state = BODY;
                }else {
                    size_t k = header_line.find(":");
                    std::string key = header_line.substr(0, k), val = header_line.substr(k+1);
                    // std::cout << key << ": " << val << std::endl;

                    request.headers[key] = val;
                }

                unparsed = unparsed.substr(j+2);
                i = 0;
            }
            break;
        case BODY:
            // std::cout << std::endl << "unparsde:: " << unparsed << std::endl;
            j = unparsed.find("\r\n\r\n");
            if(j != std::string::npos) {
                request.body += unparsed.substr(0, j);
                state = DONE;
                return true;
            }else {
                request.body += unparsed.substr(0, unparsed.size()-4);
                unparsed = unparsed.substr(unparsed.size()-4, 4);
                i = 0;
                return true;
            }
            break;
        case DONE:
            return true;
        default:
            break;
        };

    }

    return state == BODY || state == DONE;
}
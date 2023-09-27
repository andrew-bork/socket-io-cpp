#pragma once
#include <queue>
#include <string>
#include "http.hpp"
#include <iostream>

namespace http {

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
};

// static std::string trim_right(const std::string& str) {

// }

// static http::request::request_type parse_request_type(const std::string& s) {
//     if(s == "GET") {
//         return http::request::GET;
//     }else if(s == "POST") {
//         return http::request::POST;
//     }
// }

bool http::request_parser::is_done() {
    return state == DONE;
}

bool http::request_parser::is_metadata_done() {
    return state == BODY;
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

// static void to_lowercase(std::string& str) {
//     for(size_t i = 0; i < str.size(); i ++) {
//         char& c = str[i];
//         if('A' <= c && c <= 'Z') str[i] -= 'A' + 'a';
//     }
// }

bool http::request_parser::parse(const std::string& str) {
    size_t i = 0;
    bool running = true;
    while (running) {
        running = false;
        size_t j;
        std::cout << "PARSER REQUEST\n";
        unparsed += str;
        switch(state) {
        case REQUEST_LINE:
            std::cout << "State: Request line\n";
            j = unparsed.find("\r\n");
            if(j != std::string::npos) {
                std::string req_line = unparsed.substr(i, j - i);
                std::cout << "Request line: " << req_line << std::endl;
                parse_request_line(req_line, request);

                unparsed = unparsed.substr(j+2);
                i = 0;
                state = HEADERS;
            }
            break;
        case HEADERS:
            std::cout << "State: headers\n";
            j = unparsed.find("\r\n");
            if(j != std::string::npos) {
                std::string header_line = unparsed.substr(i, j - i);
                if(header_line.empty()) {
                    std::cout << "END OF HEADERS" << std::endl;
                    state = BODY;
                }else {
                    size_t k = header_line.find(":");
                    std::string key = header_line.substr(0, k), val = header_line.substr(k+1);
                    to_lowercase(key);
                    to_lowercase(val);
                    std::cout << "Header: " << key << ": " << val << std::endl;

                    request.headers[key] = val;
                }

                unparsed = unparsed.substr(j+2);
                i = 0;
            }
            break;
        case BODY:
            std::cout << std::endl << "unparsde:: " << unparsed << std::endl;
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
            std::cout << "DONE\n";
            return true;
        };

    }

    return state == BODY || state == DONE;
}
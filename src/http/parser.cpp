#include "http/parser.hpp"

bool http::request_parser::is_done() {
    return state == DONE;
}

bool http::request_parser::is_metadata_done() {
    return state == BODY;
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
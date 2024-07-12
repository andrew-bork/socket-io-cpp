#include "http/parser.hpp"
#include <iostream>
#include "util/string.hpp"
#include <stdexcept>

static bool is_alpha(char c) {
    return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

static bool is_num(char c) {
    return '0' <= c && c <= '9';
}

static bool is_unreserved(char c) {
    return is_alpha(c) || is_num(c) || c == '-' || c == '.' || c == '_' || c == '~';
}

static bool is_visible_ascii(char c) {
    return '!' <= c && c <= '~';
}



// static bool is_gen_delim(const char& c) {
//     return c == ':' || c == '/' || c == '?' || c == '#' || c == '[' || c == ']' || c == '@';
// }

static bool is_sub_delim(char c) {
    return c == '!' || c == '$' || c == '&' || c ==  '\'' || c ==  '(' || c ==  ')' || c ==  '*' || c ==  '+' || c ==  ',' || c ==  ';' || c ==  '=';
}

// static bool is_reserved(const char& c) {
//     return is_sub_delim(c) || is_sub_delim(c);
// }


static void to_lowercase(std::string& s) {
    for(size_t i = 0; i < s.size(); i ++) {
        char& curr = s[i];
        if('A' <= curr && curr <= 'Z') s[i] = curr - 'A' + 'a';
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


bool http::request_parser::is_done() {
    return state == DONE;
}

bool http::request_parser::is_header_done() {
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
    req.method = parse_request_type(parse_up_to_char(i, req_str, ' '));
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
    out.method = parse_request_type(method);
    out.url = url::url::parse_absolute_path(path);
}

static void parse_response_line(const std::string& str, http::response& out) {
    size_t i = 0;
    size_t j = str.find(' ');
    std::string version = str.substr(i, j - i);
    i = j + 1;
    j = str.find(' ', i);
    std::string status_code = str.substr(i, j - i);
    i = j + 1;
    std::string status_reason = str.substr(i);

    // std::cout << "Version: " << version << " Path: " << path << " Method: " << method << std::endl;

    out.version = out.version;
    out.status_code = std::stoi(status_code);
    out.status_reason = status_reason;
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
                    rtrim(val);
                    ltrim(val);
                    std::cout << "Header: " << key << ":" << val << std::endl;

                    request.headers[key] = val;
                }

                unparsed = unparsed.substr(j+2);
                i = 0;
            }
            break;
        case BODY:
            if(chunked) {

            }else {
                // std::cout << std::endl << "unparsde:: " << unparsed << std::endl;
                j = unparsed.find("\r\n\r\n");
                if(j != std::string::npos) {
                    // request.body += unparsed.substr(0, j);
                    state = DONE;
                    body.write(unparsed.substr(0, j));
                    body.close();

                    return true;
                }else {
                    body.write(unparsed.substr(0, unparsed.size()-4));
                    unparsed = unparsed.substr(unparsed.size()-4, 4);
                    i = 0;
                    return true;
                }
                break;
            }
        case DONE:
            std::cout << "DONE\n";
            return true;
        };

    }

    return state == BODY || state == DONE;
}



bool http::response_parser::parse(const std::string& str) {

    for(auto& c : str) {

        switch(state) {
            case VERSION:
                if(version_parser.parse(c)) {
                    std::cout << "HTTP " << version_parser.major << "." << version_parser.minor << "\n";
                    state = STATUS_CODE;
                }
                break;
            case STATUS_CODE:
                if(status_code_parser.parse(c)) {
                    std::cout << "CODE: " << status_code_parser.status_code << "\n";
                    state = STATUS_REASON;
                }
                break;
            case STATUS_REASON:
                if(status_reason_parser.parse(c)) {
                    std::cout << "Reason: " << status_reason_parser.reason << "\n";
                    state = HEADER;
                }
            case HEADER:
            case BODY:
            case DONE:
            default:
                break;
        }
        return false;
    }

    return false;

    // size_t i = 0;
    // bool running = true;
    // while (running) {
    //     running = false;
    //     size_t j;
    //     unparsed += str;
    //     switch(state) {
    //     case RESPONSE_LINE:
    //         j = unparsed.find("\r\n");
    //         if(j != std::string::npos) {
    //             std::string req_line = unparsed.substr(i, j - i);
    //             parse_response_line(req_line, response);

    //             unparsed = unparsed.substr(j+2);
    //             i = 0;
    //             state = HEADERS;
    //             running = true;
    //         }
    //         break;
    //     case HEADERS:
    //         j = unparsed.find("\r\n");
    //         if(j != std::string::npos) {
    //             std::string header_line = unparsed.substr(i, j - i);
    //             if(header_line.empty()) {
    //                 if(response.headers.contains("transfer-encoding")) {
    //                     if(response.headers["transfer-encoding"]) {
    //                         chunked = true;
    //                         length_waiting_for = 0;
    //                     }
    //                 }else if(response.headers.contains("content-length")) {
    //                     length_waiting_for = std::stoul(response.headers["content-length"]);
    //                 }
    //                 state = BODY;
    //                 running = true;
    //             }else {
    //                 size_t k = header_line.find(":");
    //                 std::string key = header_line.substr(0, k), val = header_line.substr(k+1);
    //                 to_lowercase(key);
    //                 to_lowercase(val);
    //                 rtrim(val);
    //                 ltrim(val);
    //                 std::cout << key << " : " << val << std::endl;

    //                 response.headers[key] = val;
    //                 running = true;
    //             }

    //             unparsed = unparsed.substr(j+2);
    //             i = 0;
    //         }
    //         break;
    //     case BODY:
    //         if(chunked) {
    //             if(length_waiting_for > 0) {
    //                 // Check for content length;
    //                 size_t j = unparsed.find("\r\n");
    //                 size_t chunk_length = std::stoul(unparsed.substr(0, j));
    //                 unparsed = unparsed.substr(j+2);
    //                 if(chunk_length == 0) {
    //                     state = END;
    //                 }
    //             }
    //         }else {
    //             // std::cout << std::endl << "unparsde:: " << unparsed << std::endl;
    //             j = unparsed.find("\r\n\r\n");
    //             if(j != std::string::npos) {
    //                 // request.body += unparsed.substr(0, j);
    //                 state = DONE;
    //                 body.write(unparsed.substr(0, j));
    //                 body.close();

    //                 return true;
    //             }else {
    //                 body.write(unparsed.substr(0, unparsed.size()-4));
    //                 unparsed = unparsed.substr(unparsed.size()-4, 4);
    //                 i = 0;
    //                 return true;
    //             }
    //             break;
    //         }
    //         break;
    //     case END:
    //         j = unparsed.find("\r\n");
    //         if(j != std::string::npos) {

    //         }

    //     case DONE:
    //         return true;
    //     };

    // }

    // return state == BODY || state == DONE;
}





bool http::version_parser::parse(char c) {
    switch(state){
        case EXPECT_H:
            if(c != 'H') throw std::runtime_error("Expected H, got 'C'");
            state = EXPECT_T;
            return false;
        case EXPECT_T:
            if(c != 'T') throw std::runtime_error("Expected T, got 'C'");
            state = EXPECT_TT;
            return false;
        case EXPECT_TT:
            if(c != 'T') throw std::runtime_error("Expected T, got 'C'");
            state = EXPECT_P;
            return false;
        case EXPECT_P:
            if(c != 'P') throw std::runtime_error("Expected P, got 'C'");
            state = EXPECT_SLASH;
            return false;
        case EXPECT_SLASH:
            if(c != '/') throw std::runtime_error("Expected /, got 'C'");
            state = EXPECT_MAJOR_DIGIT;
            return false;
        case EXPECT_MAJOR_DIGIT:
            if(!is_num(c)) throw std::runtime_error("Expected digit, got 'C'");
            token.push_back(c);
            state = EXPECT_MAJOR_DIGIT_OR_DOT;
            return false;

        case EXPECT_MAJOR_DIGIT_OR_DOT:
            if(is_num(c)) {
                token.push_back(c);
                return false;
            }else if(c == '.') {
                major = std::stoi(token);
                token.clear();
                state = EXPECT_MINOR_DIGIT;
                return false;
            }
            
            throw std::runtime_error("Expected digit or '.', got 'C'");
        case EXPECT_MINOR_DIGIT:
            if(!is_num(c)) throw std::runtime_error("Expected digit, got 'C'");
            token.push_back(c);
            state = EXPECT_MINOR_DIGIT_OR_SPACE;
            return false;


        case EXPECT_MINOR_DIGIT_OR_SPACE:
            if(is_num(c)) {
                token.push_back(c);
                return false;
            }else if(c == ' ') {
                minor = std::stoi(token);
                token.clear();
                state = DONE;
                return true;
            }
            
            throw std::runtime_error("Expected digit or ' ', got 'C'");
        case DONE:
            return true;
    }
    return false;
}




bool http::status_code_parser::parse(char c) {
    switch(state){
        case EXPECT_DIGIT:
            if(!is_num(c)) throw std::runtime_error("Expected digit, got 'C'");
            token.push_back(c);
            state = EXPECT_DIGIT_OR_SPACE;
            return false;

        case EXPECT_DIGIT_OR_SPACE:
            if(is_num(c)) {
                token.push_back(c);
                return false;
            }else if(c == ' ') {
                status_code = std::stoi(token);
                token.clear();
                state = DONE;
                return true;
            }
            throw std::runtime_error("Expected digit or '.', got 'C'");
        case DONE:
            return true;
    }
    return false;
}

bool http::status_reason_parser::parse(char c) {
    switch(state){
        case EXPECT_TEXT_OR_CR:
            if(c == '\r') {
                state = EXPECT_LF;
                return false;
            }
            if(is_visible_ascii(c) || c == '\t' || c == ' ') {
                reason.push_back(c);
                return false;
            }
            throw std::runtime_error("Invalid status reason character.");
        case EXPECT_LF:
            if(c == '\n') {
                state = DONE;
                return true;
            }
            throw std::runtime_error("\\r not followed by \\n");

        case DONE:
            return true;
    }
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
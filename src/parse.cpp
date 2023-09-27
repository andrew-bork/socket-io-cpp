#include <iostream>
#include <http.hpp>
#include <cstdlib>
#include "http_parser.hpp"


bool b = false;
http::request_parser parser;

void print() {
    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "Method: " << get_request_type(parser.request.type) << std::endl;
    std::cout << "Path: " << parser.request.url.path.value() << std::endl;

    std::cout << "---------------- (Query) --------------------" << std::endl;
    for(auto i = parser.request.url.queries.begin(); i != parser.request.url.queries.end(); i ++) {
        std::cout << (*i).first << " = " << (*i).second << std::endl;
    }

    std::cout << "----------------- (Headers) ------------------" << std::endl;
    for(auto i = parser.request.headers.begin(); i != parser.request.headers.end(); i ++) {
        std::cout << (*i).first << " : " << (*i).second << std::endl;
    }
    
    std::cout << "----------------- (Body) -------------------" << std::endl;
    std::cout << parser.request.body << std::endl;

}

bool test(std::string s) {
    // std::cout << "Pushed: " << s << std::endl;
    bool a = parser.parse(s);
    if(a && !b) {
        b = true;
        
        print();

    }else if(a) {
        if(!parser.request.body.empty()) {
            std::cout << parser.request.body;
            parser.request.body = "";
        }
    }

    return parser.is_done();
    // std::cout << "Finished: " << a << std::endl;
    // std::cout << "Unparsed: " << parser.unparsed << std::endl;
}


int main() {
    std::string msg = "GET / HTTP/1.1\r\nHost: localhost\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:109.0) Gecko/20100101 Firefox/117.0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate, br\r\nDNT: 1\r\nConnection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\nSec-Fetch-Dest: document\r\nSec-Fetch-Mode: navigate\r\nSec-Fetch-Site: none\r\nSec-Fetch-User: ?1\r\n\r\n";
    while(!msg.empty()) {
        size_t i = std::string::npos;
        i = std::min(i, msg.size());
        std::string s = msg.substr(0, i);
        if(test(s)) {
            std::cout << "\n\n------ FISIHED" << std::endl;
            break;
        }
        msg = msg.substr(i);
    }
}
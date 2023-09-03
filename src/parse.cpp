#include <iostream>
#include <http.hpp>
#include <cstdlib>



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
    std::string msg = "GET /ads/fdsa?fdasf=fdasfsad HTTP/1.1\r\na:b\r\nb:c\r\nc:d\r\n\r\nabc\ncde\nefg\r\nhij\r\n\r\ncock and balls lmao jfdslakjfl;dsajflkdsj";
    while(!msg.empty()) {
        size_t i = std::rand() % 30;
        i = std::min(i, msg.size());
        std::string s = msg.substr(0, i);
        if(test(s)) {
            std::cout << "\n\n------ FISIHED" << std::endl;
            break;
        }
        msg = msg.substr(i);
    }
}
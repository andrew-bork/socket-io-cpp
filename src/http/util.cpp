
#include "http/request.hpp"
#include <string>

namespace http {
    std::string get_request_type(http::request::request_type type) {
        switch(type) {
        case http::request::request_type::GET:
            return "GET";
        case http::request::request_type::POST:
            return "POST";
        }
    }
}
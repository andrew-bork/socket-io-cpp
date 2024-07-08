#pragma once

#include <string>
#include <unordered_map>
#include "http/url.hpp"

namespace http {
    struct request {
        enum request_type {
            GET, POST
        } type = GET;

        url url;

        std::string version = "HTTP/1.1";
        std::unordered_map<std::string, std::string> headers;
        std::string body = "";

        std::string build() const;
    };
}
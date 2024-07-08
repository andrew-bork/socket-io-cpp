#pragma once
#include <string>
#include <unordered_map>

namespace http {
    struct response {
        std::string version = "";
        std::string status_reason = "";
        std::string status_code = "";
        std::string body = "";
        std::unordered_map<std::string, std::string> headers;
    };
};
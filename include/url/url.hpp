#pragma once

#include <string>
#include <unordered_map>
#include <exception>
#include <optional>

#include "http/parse_error.hpp"

class percent_encoding_error : public parse_error {
    public:
        percent_encoding_error(size_t i, std::string parsed_string);
};

namespace url {
    struct url {
        std::optional<std::string> protocol = std::nullopt;
        
        std::optional<std::string> userinfo = std::nullopt;
        std::optional<std::string> domain = std::nullopt;
        std::optional<std::string> port = std::nullopt;
        std::optional<std::string> path = std::nullopt;
        std::optional<std::string> fragment = std::nullopt;

        std::unordered_map<std::string, std::string> queries;

        static url parse_absolute_path(const std::string& str);
    };
    url parse(const std::string& url_string);
}

// static protocol parse_protocol(std::string protocol_string) {
//     // protocol_string = protocol_string.÷
//     if(protocol_string == "http") {
//         return protocol::HTTP;
//     }else if(protocol_string == "https") {
//         // u.port = "443";
//         // u.protocol = url::HTTPS;
//         return protocol::HTTPS;
//     }
// }


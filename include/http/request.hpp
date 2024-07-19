#pragma once

#include <string>
#include <unordered_map>
#include "url/url.hpp"
#include <optional>
namespace http {

    struct request {
        enum request_type {
            GET, POST
        } method = GET;

        url::url url;

        std::string version = "HTTP/1.1";
        std::unordered_map<std::string, std::string> headers;
        std::optional<std::string> body = std::nullopt;

        std::string build();
    };

    std::string get_request_type(request::request_type type);
}
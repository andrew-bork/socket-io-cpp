#pragma once
#include "http/request.hpp"
#include "http/response.hpp"
#include <string_view>
namespace http {
    http::response get(const std::string& url_string);
    http::response get(const http::request req);
}
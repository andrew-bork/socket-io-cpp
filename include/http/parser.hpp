#pragma once
#include <string>

#include "http/request.hpp"
#include "http/response.hpp"
#include <functional>
#include <vector>
#include <optional>


namespace http {


    struct stream {
        std::string unparsed;

        enum events {
            DATA,
            END,
        };
        struct {
            std::vector<std::function<void(std::string)>> on_data;
            std::vector<std::function<void()>> on_end;
        } handlers;

        http::stream& on(events event, std::function<void(std::string)> handler);
        http::stream& on(events event, std::function<void()> handler);

        void write(std::string data);
        void close();
    };

    struct request_parser {
        http::request request;

        http::stream body;

        request_parser();
        bool is_header_done();
        bool is_done();
        bool parse(const std::string& str);
    };

    struct response_parser {
        http::response response;
        http::stream body;

        response_parser();
        bool parse(const std::string& response_string);
    };
}
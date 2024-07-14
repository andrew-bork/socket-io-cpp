#pragma once
#include <string>

#include "http/request.hpp"
#include "http/response.hpp"
#include <functional>
#include <vector>
#include <optional>

#include "llhttp.h"

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
        std::string header = "";
        std::string value = "";
        std::string url = "";
        bool finished = false;


        http::request request;

        llhttp_t parser;
	    llhttp_settings_t parser_settings;


        http::stream body;

        request_parser();
        bool is_header_done();
        bool is_done();
        bool parse(const std::string& str);
    };

    struct response_parser {
        std::string header = "";
        std::string value = "";
        std::string status = "";
        bool finished = false;

        http::response response;
        http::stream body;

        llhttp_t parser;
	    llhttp_settings_t parser_settings;

        response_parser();
        bool parse(const std::string& response_string);
    };
}
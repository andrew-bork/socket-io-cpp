#pragma once
#include <string>

#include "http/request.hpp"
#include "http/response.hpp"

namespace http {

    struct request_parser {
        enum State {
            REQUEST_LINE,
            HEADERS,
            BODY,
            DONE
        } state = REQUEST_LINE;

        http::request request;

        std::string unparsed = "";
        bool is_metadata_done();
        bool is_done();
        bool parse(const std::string& str);
    };

    struct response_parser {
        response response;

        enum {
            VERSION, STATUS_REASON, STATUS_CODE, HEADER_OR_BODY, HEADER_KEY, HEADER_VALUE_WHITESPACE, HEADER_VALUE, BODY, NONE, FINISHED
        } currently_parsing = VERSION;
        bool last_char_was_cr = false;

        std::string prev_unused_token = "";

        bool parse(const std::string& response_string);
    };
};
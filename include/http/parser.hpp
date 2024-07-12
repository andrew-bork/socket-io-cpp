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

    struct version_parser {
        enum {
            EXPECT_H,
            EXPECT_T,
            EXPECT_TT,
            EXPECT_P,
            EXPECT_SLASH,
            EXPECT_MAJOR_DIGIT,
            EXPECT_MAJOR_DIGIT_OR_DOT,
            EXPECT_MINOR_DIGIT,
            EXPECT_MINOR_DIGIT_OR_SPACE,
            DONE
        } state = EXPECT_H;

        std::string token = "";
        int major = 0;
        int minor = 0;

        bool parse(char c);
    };

    struct status_code_parser {
        enum {
            EXPECT_DIGIT,
            EXPECT_DIGIT_OR_SPACE,
            DONE
        } state = EXPECT_DIGIT;

        std::string token = "";
        int status_code = 0;

        bool parse(char c);
    };

    struct status_reason_parser {
        enum {
            EXPECT_TEXT_OR_CR,
            EXPECT_LF,
            DONE
        } state = EXPECT_TEXT_OR_CR;

        std::string reason = "";
        bool parse(char c);
    };

    struct request_parser {
        enum State {
            REQUEST_LINE,
            HEADERS,
            BODY,
            END,
            CHUNK_END,
            DONE
        } state = REQUEST_LINE;

        http::request request;

        bool chunked = false;


        size_t length_waiting_for = 0;
        http::stream body;

        std::string unparsed = "";
        bool is_header_done();
        bool is_done();
        bool parse(const std::string& str);
    };

    struct response_parser {
        http::response response;

        enum {
            // RESPONSE_LINE,
            // HEADERS,
            // BODY,
            // END,
            // CHUNK_END,
            // DONE
            VERSION, STATUS_CODE, STATUS_REASON, HEADER, BODY, DONE
            // VERSION, STATUS_REASON, STATUS_CODE, HEADER_OR_BODY, HEADER_KEY, HEADER_VALUE_WHITESPACE, HEADER_VALUE, BODY, NONE, FINISHED
        } state = VERSION; // = RESPONSE_LINE;
        // bool last_char_was_cr = false;


        size_t length_waiting_for = 0;
        bool chunked = false;
        
        std::string unparsed = "";

        http::stream body;

        bool parse(const std::string& response_string);
        private:
            version_parser version_parser;
            status_code_parser status_code_parser;
            status_reason_parser status_reason_parser;

    };
}
#include "http/parser.hpp"
#include <iostream>
#include "util/string.hpp"
#include <stdexcept>
#include <algorithm>

bool http::request_parser::is_done() {
    // return state == DONE;
    return false;
}

bool http::request_parser::is_header_done() {
    // return state == BODY;
    return false;
}


static http::request::request_type convert_llhttp_method(uint8_t method) {
    switch(method) {
    case HTTP_GET:
        return http::request::GET;
    case HTTP_POST:
        return http::request::POST;
    default:
        throw std::runtime_error("Method not supported");
    }
}

http::request_parser::request_parser() {
    llhttp_settings_init(&parser_settings);
    llhttp_init(&parser, HTTP_REQUEST, &parser_settings);
    parser.data = static_cast<void*> (this);

    parser_settings.on_message_complete = [](llhttp_t* parser) {
        auto& req_parser = *static_cast<http::request_parser*>(parser->data);
        req_parser.body.close();
        req_parser.finished = true;
        return 0;
    };

    parser_settings.on_body = [](llhttp_t* parser, const char * data, size_t length) {
        auto& req_parser = *static_cast<http::request_parser*>(parser->data);
        req_parser.body.write(std::string(data, length));
        return 0;
    };

    parser_settings.on_chunk_extension_value = [](llhttp_t* parser, const char * data, size_t length) {
        auto& req_parser = *static_cast<http::request_parser*>(parser->data);
        req_parser.body.write(std::string(data, length));
        return 0;
    };

    parser_settings.on_header_value_complete = [](llhttp_t* parser) {        
        auto& req_parser = *static_cast<http::request_parser*>(parser->data);
        
        std::transform(req_parser.header.begin(), req_parser.header.end(), req_parser.header.begin(),
            [](unsigned char c){ return std::tolower(c); });
        
        req_parser.request.headers[req_parser.header] = req_parser.value;
        
        req_parser.header = "";
        req_parser.value = "";
        return 0;
    };

    parser_settings.on_header_field = [](llhttp_t* parser, const char * data, size_t length) {
        auto& req_parser = *static_cast<http::request_parser*>(parser->data);
        req_parser.header += std::string(data, length);
        return 0;
    };

    parser_settings.on_header_value = [](llhttp_t* parser, const char * data, size_t length) {
        auto& req_parser = *static_cast<http::request_parser*>(parser->data);
        req_parser.value += std::string(data, length);
        return 0;
    };

    parser_settings.on_headers_complete = [](llhttp_t* parser) {
        try {
            auto& req_parser = *static_cast<http::request_parser*>(parser->data);
            req_parser.request.method = convert_llhttp_method(llhttp_get_method(parser));
            req_parser.request.url = url::parse(req_parser.url);
        }catch(const std::exception&) {
            return -1;
        }
        return 0;
    };

    parser_settings.on_url = [](llhttp_t* parser, const char * data, size_t length) {
        auto& req_parser = *static_cast<http::request_parser*>(parser->data);
        req_parser.url += std::string(data, length);
        return 0;
    };
}

http::response_parser::response_parser() {
    llhttp_settings_init(&parser_settings);
    llhttp_init(&parser, HTTP_RESPONSE, &parser_settings);
    parser.data = static_cast<void*> (this);


    parser_settings.on_message_complete = [](llhttp_t* parser) {
        // std::cout <<"huh\n";
        auto& res_parser = *static_cast<http::response_parser*>(parser->data);
        res_parser.body.close();
        res_parser.finished = true;
        return 0;
    };

    parser_settings.on_body = [](llhttp_t* parser, const char * data, size_t length) {
        auto& res_parser = *static_cast<http::response_parser*>(parser->data);
        res_parser.body.write(std::string(data, length));
        return 0;
    };

    parser_settings.on_chunk_extension_value = [](llhttp_t* parser, const char * data, size_t length) {
        auto& res_parser = *static_cast<http::response_parser*>(parser->data);
        res_parser.body.write(std::string(data, length));
        return 0;
    };

    parser_settings.on_header_value_complete = [](llhttp_t* parser) {        
        auto& res_parser = *static_cast<http::response_parser*>(parser->data);
        std::transform(res_parser.header.begin(), res_parser.header.end(), res_parser.header.begin(),
            [](unsigned char c){ return std::tolower(c); });
        res_parser.response.headers[res_parser.header] = res_parser.value;
        res_parser.header = "";
        res_parser.value = "";
        return 0;
    };

    parser_settings.on_header_field = [](llhttp_t* parser, const char * data, size_t length) {
        auto& res_parser = *static_cast<http::response_parser*>(parser->data);
        res_parser.header += std::string(data, length);
        return 0;
    };

    parser_settings.on_header_value = [](llhttp_t* parser, const char * data, size_t length) {
        auto& res_parser = *static_cast<http::response_parser*>(parser->data);
        res_parser.value += std::string(data, length);
        return 0;
    };

    parser_settings.on_headers_complete = [](llhttp_t* parser) {
        try {
            auto& res_parser = *static_cast<http::response_parser*>(parser->data);
            res_parser.response.status_code = llhttp_get_status_code(parser);
            res_parser.response.status_reason = res_parser.status;
        }catch(const std::exception&) {
            return -1;
        }
        return 0;
    };

    parser_settings.on_status = [](llhttp_t* parser, const char * data, size_t length) {
        auto& res_parser = *static_cast<http::response_parser*>(parser->data);
        res_parser.status += std::string(data, length);
        return 0;
    };

}

bool http::request_parser::parse(const std::string& str) {
    llhttp_errno err = llhttp_execute(&parser, str.data(), str.length());
    if(err == HPE_OK) {
        return finished;
    }else {
        throw std::runtime_error(std::string(llhttp_errno_name(err)) + " " + std::string(parser.reason));
    }
}


bool http::response_parser::parse(const std::string& str) {
    llhttp_errno err = llhttp_execute(&parser, str.data(), str.length());
    if(err == HPE_OK) {
        return finished;
    }else if(err == HPE_PAUSED_UPGRADE) {
        return true;
    }else {
        throw std::runtime_error(std::string(llhttp_errno_name(err)) + " " + std::string(parser.reason));
    }
}

void http::response_parser::reset() {
    header = "";
    value = "";
    status = "";
    response = http::response();
    llhttp_reset(&parser);
}


void http::request_parser::reset() {
    header = "";
    value = "";
    url = "";
    request = http::request();
    llhttp_reset(&parser);
}

http::stream& http::stream::on(events event, std::function<void(std::string)> handler) {
    switch(event) {
    case DATA:
        handlers.on_data.push_back(handler);
        for(auto handler : handlers.on_data) {
            handler(unparsed);
        }
        unparsed = "";
        break;
    default:
        throw std::runtime_error("\"CONNECT\" listener has the wrong type");
    }
    return *this;
}
http::stream& http::stream::on(events event, std::function<void()> handler) {
    switch(event) {
    case END:
        handlers.on_end.push_back(handler);
        break;
    default:
        throw std::runtime_error("\"CONNECT\" listener has the wrong type");
    }
    return *this;
}

void http::stream::write(std::string data) {
    if(unparsed.length() + data.length() > 4 * 1024 * 1024) {
        throw std::runtime_error("message too large");
    }
    unparsed += data;
    if(handlers.on_data.empty()) return;
    for(auto handler : handlers.on_data) {
        handler(unparsed);
    }
    unparsed = "";
}
void http::stream::close() {
    for(auto handler : handlers.on_end) {
        handler();
    }
}
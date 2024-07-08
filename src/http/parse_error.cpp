#include "http/parse_error.hpp"

static std::string produce_parse_error_message(size_t i, std::string parsed_string, std::string msg) {
    std::string s = msg;
    s += "\nHere:\n";

    s += '\"';
    s += parsed_string;
    s += "\"\n ";
    
    for(size_t j = 0; j < i; j ++) {
        s += ' ';
    }

    s += '^';

    return s;
}

parse_error::parse_error(size_t i, std::string parsed_string, std::string msg) : runtime_error(produce_parse_error_message(i, parsed_string, msg)) {
    // super();
}
#pragma once
#include <stdexcept>

class parse_error : public std::runtime_error{
    public:
        parse_error(size_t i, std::string parsed_string, std::string msg);
};
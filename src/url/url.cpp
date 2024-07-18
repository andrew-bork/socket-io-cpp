#include "url/url.hpp"
#include <regex>

percent_encoding_error::percent_encoding_error(size_t i, std::string parsed_string) : parse_error(i, parsed_string, "Invalid percent-escaped sequence.") {}


static bool is_alpha(const char& c) {
    return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

static bool is_num(const char& c) {
    return '0' <= c && c <= '9';
}

static bool is_unreserved(const char& c) {
    return is_alpha(c) || is_num(c) || c == '-' || c == '.' || c == '_' || c == '~';
}

// static bool is_gen_delim(const char& c) {
//     return c == ':' || c == '/' || c == '?' || c == '#' || c == '[' || c == ']' || c == '@';
// }

static bool is_sub_delim(const char& c) {
    return c == '!' || c == '$' || c == '&' || c ==  '\'' || c ==  '(' || c ==  ')' || c ==  '*' || c ==  '+' || c ==  ',' || c ==  ';' || c ==  '=';
}

static bool is_hex(const char& c) {
    return is_num(c) || c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f' || c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F';
}


struct authority {
    std::optional<std::string> userinfo, host, port;  
};

static void parse_pct_enc(std::string::const_iterator begin, std::string::const_iterator end, std::string::const_iterator& next) {
    for(size_t j = 1; j < 3; j ++) {
        if((begin + j) == end) throw std::runtime_error("percent encode error");
        if(!is_hex(*(begin + j))) throw std::runtime_error("percent encode error");
        
    }
    std::string out = std::string(begin, begin + 3);
    next = begin + 3;
}

static std::string parse_absolute_path(std::string::const_iterator begin, std::string::const_iterator end, std::string::const_iterator& next) {
    std::string out = "";
    std::string::const_iterator curr = begin;
    while(begin != end) {
        char c = *curr;
        if(c == '%') {
            parse_pct_enc(begin, end, begin);
        }else if(is_unreserved(c) || is_sub_delim(c) || c == ':' || c == '@' || c == '/') {
            curr ++;
        }else{
            break;
        }
    }
    std::string::const_iterator a = begin;
    next = curr;
    return std::string(a, curr);
}

static std::unordered_map<std::string, std::string> parse_queries(std::string::const_iterator begin, std::string::const_iterator end, std::string::const_iterator& next) {
    std::unordered_map<std::string, std::string> out;

    std::string::const_iterator value_start, curr = begin;

    while(curr != end) {
        std::string::const_iterator key_start = curr;
        // Parse key
        while(curr != end) {
            char c = *curr;
            if(c == '%') {
                parse_pct_enc(begin, end, begin);
            }else if(c == '=') { 
                // End at =
                
                curr++;
                break;
            }else if(is_unreserved(c) || is_sub_delim(c) || c == ':' || c == '@' || c == '/') {
                curr++;
            }else{
                // exit early if theres an non-query character

                next = curr;
                return out;
            }
        }

        std::string::const_iterator value_start = curr;

        while(curr != end) {
            char c = *curr;
            if(c == '%') {
                parse_pct_enc(begin, end, begin);
            }else if(c == '&') {
                // End at &
                // Add key-value to output map.
                out[std::string(key_start, value_start-2)] = std::string(value_start, curr);

                curr++;
                break;
            }else if(is_unreserved(c) || is_sub_delim(c) || c == ':' || c == '@' || c == '/') {
                curr++;
            }else{
                // exit early if theres an non-query character
                // Add key-value to output map.
                out[std::string(key_start, value_start-2)] = std::string(value_start, curr);
                next = curr;
                return out;
            }
        }
    }

    return out;
}

static std::regex scheme_matcher("\\w[\\w\\d+\\-.]*:");
static std::regex authority_matcher("([\\w\\d\\-._~%!$&'()*+,/=:]+@)?([\\w\\d\\-._~%!$&'()*+,;=]+)(:\\d*)?");

static std::optional<std::string> parse_scheme(std::string::const_iterator begin, std::string::const_iterator end, std::string::const_iterator& next) {
    std::smatch match;
    if(std::regex_search(begin, end, match, scheme_matcher)) {
        next = match[0].second;
        return std::string(match[0].first, match[0].second-1);
    }
    return std::nullopt;
}

// static std::optional<std::string> parse

static authority parse_authority(std::string::const_iterator begin, std::string::const_iterator end, std::string::const_iterator& next) {
    authority out;
    std::smatch match;
    if(std::regex_search(begin, end, match, authority_matcher)) {
        next = match[0].second;
        if(match[3].length() > 0) out.port = std::string(match[3].first+1, match[3].second);
        out.host = match[2].str();
        if(match[1].length() > 0) out.userinfo = std::string(match[1].first, match[1].second - 1);
    }
    return out;
}


url::url url::parse(const std::string& url_string) {
    url returned;

    // Initialize Iterators
    std::string::const_iterator current = url_string.begin(), end = url_string.end();
    
    // Parse Scheme "https:..."
    returned.protocol = parse_scheme(current, end, current);

    // Parse authority if "//..."
    if(*current == '/' && *(current + 1) == '/') {
        current = current + 2;

        authority a = parse_authority(current, end, current);
        returned.userinfo = a.userinfo;
        returned.host = a.host;
        returned.port = a.port;
    }

    // Parse path
    returned.path = parse_absolute_path(current, end, current);
    
    // Parse query if "?...."
    if(*current == '?') {
        current ++;
        returned.queries = parse_queries(current, end, current);
    }

    // Parse fragment/anchor if "#......"
    if(*current == '#') {
        current ++;
        returned.fragment = std::string(current, end);
    }

    return returned;
}


std::string url::url::get_host() {
    if(!host.has_value()) throw std::runtime_error("Host has no value.");
    if(port.has_value()) {
        return host.value() + ":" + port.value();
    }
    return host.value();
}
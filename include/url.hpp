#ifndef URL_DEFINED
#define URL_DEFINED

#include <string>
#include <unordered_map>
#include <exception>


class parse_error : public std::runtime_error{
    public:
        parse_error(size_t i, std::string parsed_string, std::string msg);
};

class percent_encoding_error : public parse_error {
    public:
        percent_encoding_error(size_t i, std::string parsed_string);
};

percent_encoding_error::percent_encoding_error(size_t i, std::string parsed_string) : parse_error(i, parsed_string, "Invalid percent-escaped sequence.") {}

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

struct url {
    std::optional<std::string> protocol = std::nullopt;
    
    std::optional<std::string> userinfo = std::nullopt;
    std::optional<std::string> domain = std::nullopt;
    std::optional<std::string> port = std::nullopt;
    std::optional<std::string> path = std::nullopt;
    std::optional<std::string> fragment = std::nullopt;

    std::unordered_map<std::string, std::string> queries;

    static std::optional<url> parse(const std::string& url_string);
    static url parse_absolute_path(const std::string& str);
};


// static protocol parse_protocol(std::string protocol_string) {
//     // protocol_string = protocol_string.÷
//     if(protocol_string == "http") {
//         return protocol::HTTP;
//     }else if(protocol_string == "https") {
//         // u.port = "443";
//         // u.protocol = url::HTTPS;
//         return protocol::HTTPS;
//     }
// }

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

// static bool is_reserved(const char& c) {
//     return is_sub_delim(c) || is_sub_delim(c);
// }


static void to_lowercase(std::string& s) {
    for(size_t i = 0; i < s.size(); i ++) {
        char& curr = s[i];
        if('A' <= curr && curr <= 'Z') s[i] = curr - 'A' + 'a';
    }
}

static std::optional<std::string> parse_protocol(size_t& i, const std::string& url_string) {
    size_t orginal_position = i;
    std::string out = "";

    if(i < url_string.size() && is_alpha(url_string[i])) {
        out += url_string[i];
        i++;
    }else {
        i = orginal_position;
        return std::nullopt;
    }

    while(i < url_string.size()) {
        char curr = url_string[i];
        if(curr == ':') {
            i++;

            to_lowercase(out);

            return out;
        }

        if(is_alpha(curr) || is_num(curr) || curr == '+' || curr == '-' || curr == '.') {
            out += curr;
            i ++;
        }else {
            i = orginal_position;
            return std::nullopt;
        }
    }

    i = orginal_position;
    return std::nullopt;
}

static bool is_hex(const char& c) {
    return is_num(c) || c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f' || c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F';
}

static std::optional<std::string> parse_ipv4(size_t& i, const std::string& url_string) {
    return std::nullopt;
}
static std::optional<std::string> parse_ipv6(size_t& i, const std::string& url_string) {
    return std::nullopt;
}

static std::optional<std::string> parse_pct_escp(size_t& i, const std::string& url_string) {
    for(size_t j = 1; j < 3; j ++) {
        if((i + j) >= url_string.size()) throw percent_encoding_error(i+j, url_string);
        if(!is_hex(url_string[i+j])) throw percent_encoding_error(i+j, url_string);

    }
    std::string out = url_string.substr(i, 3);
    i += 3;
    return out;
}

static std::optional<std::string> parse_regname(size_t& i, const std::string& url_string) {

    std::string out = "";

    while(i < url_string.size()) {
        char curr = url_string[i];
        if(curr == '%') {
            auto pct_escp = parse_pct_escp(i, url_string);
            if(!pct_escp.has_value()) {
                return std::nullopt;
            }

            out += pct_escp.value();
        }else if(is_unreserved(curr) || is_sub_delim(curr)) {
            out += curr;
            i++;
        }else {
            return out;
        }
    }

    return out;
}

static std::optional<std::string> parse_userinfo(size_t& i, const std::string& url_string) {
    size_t orginal_position = i;
    std::string out = "";

    while(i < url_string.size()) {
        char curr = url_string[i];
        if(curr == '%') {
            auto pct_escp = parse_pct_escp(i, url_string);
            if(!pct_escp.has_value()) {
                return std::nullopt;
            }

            out += pct_escp.value();
        }else if(is_unreserved(curr) || is_sub_delim(curr) || curr == ':') {
            out += curr;
            i++;
        }else if(curr == '@') {
            i++;
            return out;
        }else{
            i = orginal_position;
            return std::nullopt;
        }
    }

    i = orginal_position;
    return std::nullopt;
}


static std::optional<std::string> parse_host(size_t& i, const std::string& url_string) {
    auto ipv4 = parse_ipv4(i, url_string);
    if(ipv4.has_value()) {
        return ipv4;
    }

    auto ipv6 = parse_ipv6(i, url_string);
    if(ipv6.has_value()) {
        return ipv6;
    }

    auto regname = parse_regname(i, url_string);
    if(regname.has_value()) {
        return regname;
    }

    return std::nullopt;
}


static std::optional<std::string> parse_port(size_t& i, const std::string& url_string) {
    size_t orginal_position = i;
    std::string out = "";

    if(i < url_string.size() && url_string[i] == ':') {
        i++;
    }else {
        i = orginal_position;
        return std::nullopt;
    }

    while(i < url_string.size()) {
        char curr = url_string[i];

        if(is_num(curr)) {
            out += curr;
            i++;
        }else{
            return out;
        }
    }

    return out;
}

struct authority {
    std::optional<std::string> userinfo, host, port;  
};

static authority parse_authority(size_t& i, const std::string& url_string) {

    authority out;

    out.userinfo = parse_userinfo(i, url_string);
    
    out.host = parse_host(i, url_string);
    
    out.port = parse_port(i, url_string);
    
    return out;
}

static std::optional<std::string> parse_absolute_path(size_t& i, const std::string& url_string) {
    size_t orginal_position = i;
    std::string out = "";

    if(i < url_string.size() && url_string[i] == '/') {
        out += url_string[i];
        i++;
    }else {
        i = orginal_position;
        return std::nullopt;
    }

    
    while(i < url_string.size()) {
        char curr = url_string[i];
        if(curr == '%') {
            auto pct_escp = parse_pct_escp(i, url_string);
            if(!pct_escp.has_value()) {
                return std::nullopt;
            }

            out += pct_escp.value();
        }else if(is_unreserved(curr) || is_sub_delim(curr) || curr == ':' || curr == '@' || curr == '/') {
            out += curr;
            i++;
        }else{
            return out;
        }
    }

    return out;
}

static std::unordered_map<std::string, std::string> parse_queries(size_t& i, const std::string& url_string) {
    size_t orginal_position = i;
    std::unordered_map<std::string, std::string> out;

    if(i < url_string.size() && url_string[i] == '?') {
        i++;
    }else {
        i = orginal_position;
        return out;
    }

    std::string key = "";
    std::string value = "";
    bool parsing_key = true;
    while(i < url_string.size()) {
        char curr = url_string[i];
        if(curr == '%') {
            auto pct_escp = parse_pct_escp(i, url_string);
            if(!pct_escp.has_value()) {
                return out;
            }

            if(parsing_key) key += pct_escp.value();
            else value += pct_escp.value();
        }else if(curr == '&') { 
            if(!parsing_key) {
                out[key] = value;
                parsing_key = true;
            }
            key = "";
            value = "";
            i++;
        }else if(curr == '=') {
            parsing_key = false;
            i++;
        }else if(is_unreserved(curr) || is_sub_delim(curr) || curr == ':' || curr == '@' || curr == '/') {
            if(parsing_key) key += curr;
            else value += curr;
            i++;
        }else{
            break;
        }
    }
    
    if(!parsing_key) {
        out[key] = value;
        parsing_key = true;
    }

    return out;
}

static std::optional<std::string> parse_fragment(size_t& i, const std::string& url_string) {
    std::string out;

    if(i < url_string.size() && url_string[i] == '#') {
        i++;
    }else {
        return std::nullopt;
    }


    while(i < url_string.size()) {
        char curr = url_string[i];
        if(curr == '%') {
            auto pct_escp = parse_pct_escp(i, url_string);
            if(!pct_escp.has_value()) {
                return std::nullopt;
            }

            out += pct_escp.value();
        }else if(is_unreserved(curr) || is_sub_delim(curr) || curr == ':' || curr == '@' || curr == '/' || curr == '?') {
            out += curr;
            i++;
        }else{
            return out;
        }
    }

    return out;;
}

// static std::optional<std::string> get_default_port(const std::string& protocol) {
//     if(protocol == "https") {
//         return "443";
//     }else if(protocol == "http") {
//         return "80";
//     }

//     return  std::nullopt;
// }


url url::parse_absolute_path(const std::string& str) {
    url out;
    size_t i = 0;

    auto path = ::parse_absolute_path(i, str);
    if(!path.has_value()) throw parse_error(0, str, "String has no absolute path.");
    out.path = path;

    out.queries = parse_queries(i, str);

    out.fragment = parse_fragment(i, str);

    return out;
}

std::optional<url> url::parse(const std::string& url_string) {
    // https://video.google.co.uk:80/videoplay?docid=-7234293487129834&hl=en#00h02m30s
    url returned;

    size_t i = 0;

    returned.protocol = parse_protocol(i, url_string);

    if(i + 1 < url_string.size() && url_string[i] == '/' && url_string[i+1] == '/') {
        i += 2;
        authority auth = parse_authority(i, url_string);

        returned.domain = auth.host;
        returned.userinfo = auth.userinfo;
        returned.port = auth.port;
    }

    returned.path = ::parse_absolute_path(i, url_string);

    returned.queries = parse_queries(i, url_string);

    returned.fragment = parse_fragment(i, url_string);

    return returned;
}

#endif
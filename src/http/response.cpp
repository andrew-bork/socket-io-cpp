


http::response http::response::parse(const std::string& response_string) {
    size_t i = 0;
    http::response out;
    parse_status_line(i, response_string, out);

    parse_headers(i, response_string, out.headers);
    
    std::cout << "v: " << out.version << " code: " << out.status_code << " reason: " << out.status_reason << std::endl;

    out.body = response_string.substr(i);
    return out;
}
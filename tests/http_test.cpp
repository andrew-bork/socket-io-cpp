#include <iostream>
#include <http.hpp>


void test(std::string s) {
    std::cout << "Parsing: " << s << std::endl;
    url _url = url::parse(s).value();
    std::cout <<    "Protocol: " << _url.protocol.value_or("NONE") << 
                    " Domain: " << _url.domain.value_or("NONE") <<
                    " Port: " << _url.port.value_or("NONE") << std::endl << 
                    "User Info: " << _url.userinfo.value_or("NONE") << std::endl << 
                    "Path: " << _url.path.value_or("NONE") << std::endl << 
                    "Fragment: " << _url.fragment.value_or("NONE") << std::endl <<
                    "Queries: " << std::endl;

    for(auto i = _url.queries.begin(); i != _url.queries.end(); i ++) {
        std::cout << (*i).first << " = " << (*i).second << std::endl;
    }

}

int main() {
    test("https://video.google.co.uk/videoplay?docid=-7234293487129834&hl=en#00h02m30s");
    test("https://video.google.co.uk:2321/videoplay?docid=-7234293487129834&hl=en#00h02m30s");
    test("http://video.google.co.uk/videoplay?docid=-7234293487129834&hl=en#00h02m30s");
    test("mailto:video.google.co.uk/videoplay?docid=-7234293487129834&hl=en#00h02m30s");
    test("mailto:bork.andrew.ryan@gmail.com");
    test("https://google.com");
    test("https://google.com/");

    http::request req;
    req.url = url::parse("http://google.com/hello/world?a=b").value();
    req.headers["lang"] = "eng-pls";
    req.body = "goo goo gaa gaa";

    std::cout << req.build() << std::endl;

    // std::cout << http::curl("http://www.google.com/") << std::endl;
    http::response::parse(http::curl("http://www.google.com/"));
}
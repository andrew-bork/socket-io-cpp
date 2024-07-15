#include <iostream>
#include "url/url.hpp"
#include "http/request.hpp"


void test(std::string s) {
    std::cout << "Parsing: \"" << s << "\"" << std::endl;
    url::url _url = url::parse(s);
    std::cout <<    "\tProtocol: " << _url.protocol.value_or("NONE") <<  std::endl << 
                    "\tDomain: " << _url.host.value_or("NONE") << std::endl << 
                    "\tPort: " << _url.port.value_or("NONE") << std::endl << 
                    "\tUser Info: " << _url.userinfo.value_or("NONE") << std::endl << 
                    "\tPath: " << _url.path.value_or("NONE") << std::endl << 
                    "\tFragment: " << _url.fragment.value_or("NONE") << std::endl <<
                    "\tQueries: " << std::endl;
    if(_url.queries.size() == 0) std::cout <<"\t\tNONE\n";
    for(auto i = _url.queries.begin(); i != _url.queries.end(); i ++) {
        std::cout << "\t\t" << (*i).first << " = " << (*i).second << std::endl;
    }
    std::cout << std::endl << std::endl;

}

int main() {
    test("https://video.google.co.uk/videoplay?docid=-7234293487129834&hl=en#00h02m30s");
    test("https://video.google.co.uk:2321/videoplay?docid=-7234293487129834&hl=en#00h02m30s");
    test("http://video.google.co.uk/videoplay?docid=-7234293487129834&hl=en#00h02m30s");
    test("mailto://video.google.co.uk/videoplay?docid=-7234293487129834&hl=en#00h02m30s");
    test("mailto:bork.andrew.ryan@gmail.com");
    test("https://google.com");
    test("https://google.com/");
    test("https://a-321@b.cdfdas.com:890/");
    test("//www.google.com/");
    test("https://jisho.org/search/%E6%89%93%E3%81%A1");

    http::request req;
    req.url = url::parse("http://google.com/hello/world?a=b");
    req.headers["lang"] = "eng-pls";
    req.body = "lorem ipsum dolor";

    std::cout << req.build() << std::endl;

    // std::cout << http::curl("http://www.google.com/") << std::endl;
    // http::response::parse(http::curl("http://www.google.com/"));
}
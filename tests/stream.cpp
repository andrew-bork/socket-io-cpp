#include "stream/writable.hpp"
#include "unistd.h"
#include <ctype.h>
class print_stream : public stream::writable {
    size_t _buffer_size = 0;
    size_t _i = 0;

    char* _buffer = NULL;
    // char buf[4096];
    bool _write(std::span<const char> data) {
        bool should_drain = false;
        for(auto c : data) {
            _buffer[_i++] = c;
            if(c == '\n') {
                should_drain = true;
            }else if(_i == _buffer_size) {
                _drain();
            }


        }
        if(should_drain) return _drain();
        return false;
    }

    bool _drain() { 
        ::write(STDOUT_FILENO, _buffer, _i);
        _i = 0;
        return true; 
    }

    public:
        print_stream(size_t N = 4096) {
            _buffer_size = N;
            if(_buffer != NULL) delete _buffer;
            _buffer = new char[_buffer_size];
            _i = 0;
        }
        ~print_stream() {
            _drain();
            if(_buffer != NULL) delete _buffer;
        }
};

int main() {
    print_stream printer;
    stream::writable& w = printer;
    w.write("Hello World!");
}
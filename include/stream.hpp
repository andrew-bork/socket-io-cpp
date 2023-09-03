#pragma once

#include <string>

namespace stream {
    class readable {
        public:
            size_t read(size_t n_bytes, std::string& out) {
                
            }
    };

    class writable {
        public:
            void write(const std::string& input) {

            }
    };

    class duplex : public readable, writable {

    };
}
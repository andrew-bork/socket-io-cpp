#pragma once

#include <string>
#include <span>
#include <string_view>
#include <cstring>

namespace stream {
    class writable {
        public: 
            // bool write(const std::string& data);
            inline bool write(std::span<const char> data)  {
                return _write(data);
            }
            inline bool write(std::string_view data)  {
                return _write(std::span<const char>(data.begin(), data.end()));
            }
            inline bool write(const char* data) {
                size_t s = strlen(data);
                return _write(std::span<const char>(data, s));
            }
            // bool write(const char* data);
            // bool write();
            // bool write();
            // bool write();

        private:
            virtual bool _write(std::span<const char> data) = 0;
            virtual bool _drain() = 0;
    };
};
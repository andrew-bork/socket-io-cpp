#pragma once
#include <string>

namespace socketio {

    struct server {

        void emit(const std::string& event);
        void close();
        void on(const std::string& event);

    };

};
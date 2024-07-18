#pragma once

#include <string>
#include <span>
#include <string_view>
#include <cstring>
#include <functional>
#include <list>
#include "util/callback.hpp"

namespace stream {
    // enum events {

    // };
    class readable {
        public: 
            typedef std::function<void(std::span<const char>)> on_data_handler;
            typedef std::function<void(void)> on_close_handler;

            struct {
                callback_list<on_data_handler> on_data;
                callback_list<on_close_handler> on_close;
            } handlers;

            inline callback_list<on_data_handler>::callback_manager on_data(on_data_handler on_data) {
                return handlers.on_data.add(on_data);
            }
            inline callback_list<on_close_handler>::callback_manager on_close(on_close_handler on_close) {
                return handlers.on_close.add(on_close);
            }
            
        private:
            // virtual bool _read(std::span<const char> data) = 0;
            // virtual bool _read() = 0;
    };
};
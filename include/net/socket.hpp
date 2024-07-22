#pragma once

#include <vector>
#include <functional>
#include <string>

#include <list>

#include "util/callback.hpp"
#include "stream/readable.hpp"
#include "stream/writable.hpp"
#include "net/event_loop.hpp"

#include "ev.h"


namespace net {
    class socket : public stream::writable, public stream::readable, public net::watchable {
        public:
            typedef std::function<void(void)> on_connect_handler;
            typedef std::function<void(void)> on_disconnect_handler;
            enum events {
                CONNECT,
                DATA,
                DISCONNECT,
            };

            socket(int _fd);
            socket(const socket& other);
            socket(socket&& other);
            ~socket();

            
            struct {
                callback_list<on_connect_handler> on_connect;
                callback_list<on_disconnect_handler> on_disconnect;
            } handlers;


            // template<events E, typename F>
            // void on(F f) {}

            // template<>
            // void on<events::CONNECT>(std::function<void()> f);
            // template<>
            // void on<events::DATA>(std::function<void(std::string)> f);
            // template<>
            // void on<events::DISCONNECT>(std::function<void()> f);

            callback_list<on_connect_handler>::callback_manager on_connect(on_connect_handler f);
            // callback_list<on_data_handler> on_data(on_data_handler f);
            callback_list<on_disconnect_handler>::callback_manager on_disconnect(on_disconnect_handler f);

            net::socket& on(events event, std::function<void()> handler);
            net::socket& on(events event, stream::readable::on_data_handler handler);

            inline int& fd() {
                return _fd;
            }
            void close();

            size_t operator>>(std::string& string);
            size_t operator<<(const std::string string);
        
            net::socket& operator=(net::socket&&);
            net::socket& operator=(const net::socket&);


            inline net::event_loop* loop() {
                return _loop;
            }

        private:
            int _fd = -1;
            // int i = -1;

            bool connected = false;
            bool opened = false;

            size_t _send_buffer_size = 4096;
            size_t _send_buffer_i = 0;
            char* _send_buffer = NULL;

            
            // struct ev_loop* _loop;

            net::event_loop* _loop;
            ev_io _read_watcher, _write_watcher;

            void _initialize_watchers();
            void _destroy_watchers();

            void _initialize_buffers();
            void _destroy_buffers();

            bool _write(std::span<const char> data);
            bool _drain();

            void _attach();
            void _detach();

            // static void _on_readable(EV_P_ ev_io*w, int revents);
            // static void _on_writable(EV_P_ ev_io*w, int revents);
    };
};
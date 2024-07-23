#pragma once

#include <list>
#include <chrono>
#include <vector>

#include "ev.h"

namespace net {

    struct event_loop;

    class watchable{
            typedef typename std::list<watchable*>::iterator watcher_iterator;
        private:
            watcher_iterator _iterator;
            bool _attached = false;
            event_loop* _loop;
            virtual void _attach() = 0;
            virtual void _detach() = 0;
        public:
            inline bool is_attached() { return _attached; }
            inline event_loop* loop() {
                return _loop;
            }
            inline void attach(event_loop& loop); // {
            //     _loop->add(*this);
            // }
            inline void detach(); // {
            //     _loop->remove(*this);
            // }
            inline ~watchable(); // {
                // detach();
                // _loop->remove(*this);

            // }

            friend event_loop;
    };

    struct event_loop {

        event_loop();
        
        // typedef typename std::list::<socket_watvh socket_watcher_iter

        // void add(net::socket& socket);
        // void add(net::server& server);

        // friend watchable;

        friend watchable;

        inline void add(watchable& _watchable) {
            _watchable._loop = this;
            _watchable._attach();
            _watchables.push_back(&_watchable);
            _watchable._iterator = --_watchables.end();
            _watchable._attached = true;
        }

        inline void remove(watchable& _watchable) {
            if(_watchable._loop != this) throw std::runtime_error("Watchable removed from event loop it wasn't attached to.");
            _watchable._detach();
            _watchables.erase(_watchable._iterator);
            _watchable._attached = false;
        }

        void run();
        void run(std::chrono::duration<std::milli> timeout);

        inline void add_watcher(ev_io& watcher) {
            ev_io_start(_loop, &watcher);
        }

        inline void remove_watcher(ev_io& watcher) {
            ev_io_stop(_loop, &watcher);
        }

        private: 


        
            // struct socket_watcher : public watcher {
            //     net::socket& socket;
            //     struct ev_loop* loop;
            //     ev_io read_watcher, write_watcher;


            //     socket_watcher(struct ev_loop* _loop, net::socket& _socket);
            //     ~socket_watcher();

            //     static void on_readable(EV_P_ ev_io*w, int revents);
            //     static void on_writable(EV_P_ ev_io*w, int revents);
            // };

            // struct server_watcher {
            //     net::server& server;
            //     struct ev_loop* loop;
            //     ev_io read_watcher;


            //     server_watcher(struct ev_loop* _loop, net::server& _server);
            //     ~server_watcher();

            //     static void on_readable(EV_P_ ev_io*w, int revents);
            // };

            struct ev_loop* _loop;
            std::list<watchable*> _watchables;
            // std::list<socket_watcher> _socket_watchers;
            // std::list<server_watcher> _server_watchers;

    };
};

void net::watchable::attach(net::event_loop& loop) {
    loop.add(*this);
}
void net::watchable::detach() {
    if(_loop == NULL) throw std::runtime_error("Watchable wasn't attached");
    _loop->remove(*this);
}
inline net::watchable::~watchable() {
    if(_loop == NULL) return;
    // _watchable._detach();
    _loop->_watchables.erase(_iterator);
    _attached = false;
}
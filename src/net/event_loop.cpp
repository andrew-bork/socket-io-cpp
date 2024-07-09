#include "net/event_loop.hpp"

net::event_loop::event_loop() : _loop(ev::get_default_loop()) {

}


void net::event_loop::add(std::shared_ptr<net::socket> socket) {
    // auto& watcher = _watchers.(_loop, socket);
    net::event_loop::socket_watcher watcher(_loop, socket);
    _watchers.push_back(watcher);
}

void net::event_loop::add(std::shared_ptr<net::server> server) {

}

void net::event_loop::run() {
    _loop.run();
}

void net::event_loop::run(std::chrono::duration<std::milli> timeout) {
    _loop.run();

}

net::event_loop::socket_watcher::socket_watcher(ev::loop_ref loop, std::shared_ptr<net::socket> _socket)
     : socket(_socket), read_watcher(loop) {
    read_watcher.set<net::event_loop::socket_watcher, &net::event_loop::socket_watcher::on_data>(this);
    read_watcher.set(_socket->fd, ev::READ);
    read_watcher.start();
}

void net::event_loop::socket_watcher::on_connect(ev::io &w, int revents) {
    for(auto& handler : socket->on_connect_handlers) {
        handler();
    }
}

void net::event_loop::socket_watcher::on_data(ev::io &w, int revents) {
    std::string data;
    ssize_t result = (*socket) >> data;
    if(result == 0) {
        on_disconnect(w, revents);
    }else {
        for(auto& handler : socket->on_data_handlers) {
            handler(data);
        }
    }
}

void net::event_loop::socket_watcher::on_disconnect(ev::io &w, int revents) {
    for(auto& handler : socket->on_disconnect_handlers) {
        handler();
    }
}
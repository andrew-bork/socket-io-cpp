#include "net/event_loop.hpp"

net::event_loop::event_loop() : _loop(EV_DEFAULT) {

}


void net::event_loop::add(std::shared_ptr<net::socket> socket) {
    // auto& watcher = _watchers.(_loop, socket);
    _watchers.emplace_back(_loop, socket);
}

void net::event_loop::add(std::shared_ptr<net::server> server) {

}

void net::event_loop::run() {
    ev_run(_loop, 0);
}


// struct timeout_watcher {
//     ev_timer
// };

// static void timeout_callback() {
//     ev_break();
// }

void net::event_loop::run(std::chrono::duration<std::milli> timeout) {
    ev_run(_loop);

}

net::event_loop::socket_watcher::~socket_watcher() {
    if(ev_is_active(&read_watcher)) ev_io_stop(loop, &read_watcher);
    if(ev_is_active(&write_watcher)) ev_io_stop(loop, &write_watcher);
}

net::event_loop::socket_watcher::socket_watcher(struct ev_loop* _loop, std::shared_ptr<net::socket> _socket)
     : loop(_loop), socket(_socket) {
    ev_io_init(&read_watcher, &net::event_loop::socket_watcher::on_readable, _socket->fd, EV_READ);
    read_watcher.data = (void*) this;
    ev_io_start(loop, &read_watcher);

    ev_io_init(&write_watcher, &net::event_loop::socket_watcher::on_writable, _socket->fd, EV_WRITE);
    write_watcher.data = (void*) this;
    ev_io_start(loop, &write_watcher);


}

void net::event_loop::socket_watcher::on_writable(EV_P_ ev_io* w, int revents) {
    auto watcher = (net::event_loop::socket_watcher *) w->data;
    auto socket = watcher->socket;
    ev_io_stop(watcher->loop, w);
    for(auto& handler : socket->on_connect_handlers) {
        handler();
    }
}

void net::event_loop::socket_watcher::on_readable(EV_P_ ev_io* w, int revents) {
    auto watcher = (net::event_loop::socket_watcher *) w->data;
    auto socket = watcher->socket;
    std::string data;
    ssize_t result = (*socket) >> data;
    if(result == 0) {
        for(auto& handler : socket->on_disconnect_handlers) {
            handler();
        }
        ev_io_stop(watcher->loop, w);
    }else {
        for(auto& handler : socket->on_data_handlers) {
            handler(data);
        }
    }
}







net::event_loop::server_watcher::~server_watcher() {
    if(ev_is_active(&read_watcher)) ev_io_stop(loop, &read_watcher);
}

net::event_loop::server_watcher::server_watcher(struct ev_loop* _loop, std::shared_ptr<net::server> _socket)
     : loop(_loop), socket(_socket) {
    ev_io_init(&read_watcher, &net::event_loop::server_watcher::on_readable, _socket->fd, EV_READ);
    read_watcher.data = (void*) this;
    ev_io_start(loop, &read_watcher);


}

void net::event_loop::server_watcher::on_readable(EV_P_ ev_io* w, int revents) {
    auto watcher = (net::event_loop::server_watcher *) w->data;
    auto server = watcher->server;

    int client_fd = accept(server->fd(), NULL, NULL);
    if(client_fd < 0) {
        throw std::runtime_error("Accept failed");
    }
    
    

    for(auto handler : server->handlers.on_connect) {
        handler();
    }
}
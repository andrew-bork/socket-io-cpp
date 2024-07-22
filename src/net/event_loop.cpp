#include "net/event_loop.hpp"
#include <iostream>
net::event_loop::event_loop() : _loop(EV_DEFAULT) {

}

void net::event_loop::run() {
    ev_run(_loop, 0);
}



void net::event_loop::run(std::chrono::duration<std::milli> timeout) {
    ev_run(_loop);

}

// net::event_loop::socket_watcher::~socket_watcher() {
//     if(ev_is_active(&read_watcher)) ev_io_stop(loop, &read_watcher);
//     if(ev_is_active(&write_watcher)) ev_io_stop(loop, &write_watcher);
// }

// net::event_loop::socket_watcher::socket_watcher(struct ev_loop* _loop, net::socket& _socket)
//      : socket(_socket), 
//      loop(_loop)
//      {
//     ev_io_init(&read_watcher, &net::event_loop::socket_watcher::on_readable, _socket.fd(), EV_READ);
//     read_watcher.data = static_cast<void*>(this);
//     ev_io_start(loop, &read_watcher);

//     ev_io_init(&write_watcher, &net::event_loop::socket_watcher::on_writable, _socket.fd(), EV_WRITE);
//     write_watcher.data = static_cast<void*>(this);
//     ev_io_start(loop, &write_watcher);


// }

// void net::event_loop::socket_watcher::on_writable(EV_P_ ev_io* w, int revents) {
//     auto watcher = static_cast<net::event_loop::socket_watcher*>(w->data);
//     auto& socket = watcher->socket;
//     ev_io_stop(watcher->loop, w);
//     socket.handlers.on_connect.call();
// }

// void net::event_loop::socket_watcher::on_readable(EV_P_ ev_io* w, int revents) {
//     auto watcher = static_cast<net::event_loop::socket_watcher*>(w->data);
//     auto& socket = watcher->socket;
//     stream::readable& readable = socket;
//     std::string data;
//     ssize_t result = socket >> data;
//     if(result == 0) {
//         socket.handlers.on_disconnect.call();
//         readable.handlers.on_close.call();
//         socket.close();
//         ev_io_stop(watcher->loop, w);
//     }else {
//         readable.handlers.on_data.call(std::span<const char>(data.begin(), data.end()));
//         if(socket.fd() == -1) ev_io_stop(watcher->loop, w);
//     }
// }







// net::event_loop::server_watcher::~server_watcher() {
//     if(ev_is_active(&read_watcher)) ev_io_stop(loop, &read_watcher);
// }

// net::event_loop::server_watcher::server_watcher(struct ev_loop* _loop, net::server& _server)
//      : server(_server), loop(_loop) {
//     ev_io_init(&read_watcher, &net::event_loop::server_watcher::on_readable, _server.fd(), EV_READ);
//     read_watcher.data = static_cast<void*>(this);
//     ev_io_start(loop, &read_watcher);


// }

// void net::event_loop::server_watcher::on_readable(EV_P_ ev_io* w, int revents) {
//     auto watcher = static_cast<net::event_loop::server_watcher*>(w->data);
//     auto& server = watcher->server;

//     int client_fd = accept(server.fd(), NULL, NULL);
//     if(client_fd < 0) {
//         throw std::runtime_error("Accept failed");
//     }
    
//     auto& client = server._connections.emplace_back(net::socket(client_fd));

//     for(auto handler : server.handlers.on_connect) {
//         handler(client);
//     }

//     if(server.fd() == -1 || !server.listening) ev_io_stop(watcher->loop, w);
// }
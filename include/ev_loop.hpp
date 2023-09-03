#pragma once

#include <sys/event.h>
#include <exception>
#include <vector>
#include <memory>
#include <socket.hpp>

struct eventable {
    enum type {
        SOCKET,
        SERVER

    } type;

    union eventable_ptrs {
        std::shared_ptr<net::server>,
        std::shared_ptr<net::socket>
    } eventable_ptr;
};

struct event_loop {
    int kqueue_fd =  -1;
    bool running = false;

    std::vector<kevent> events;
    std::vector<kevent> triggered;

    std::vector<eventable> eventables;

    event_loop();

    void start();
    void add(std::shared_ptr<net::server> server);
    void add(std::shared_ptr<net::socket> socket);
}

event_loop::event_loop() {
    kqueue_fd = kqueue();
    if(kqueue_fd == -1) throw std::runtime_error("Kqueue init failed");
}

void event_loop::start() {
    while(running) {
        int n_events = kevent(kqueue_fd, &(events[0]), events.size(), &(triggered[0]), triggered.size(), NULL);
        if(n_events == -1) throw std::runtime_error("Kevent call failed");


    }
}

void event_loop::add(std::shared_ptr<net::server> server) {
    eventable ev;
    ev.type = eventable::SERVER;
    ev.eventable_ptr = server;

    kevent kev;
    EV_SET(&kev, server->_socket.fd, EVFILT_READ, EV_ADD, 0, 0, 0);

    triggered.push_back(kevent());
}
void event_loop::add(std::shared_ptr<net::socket> socket) {
    eventable ev;
    ev.type = eventable::SOCKET;
    ev.eventable_ptr = socket;

    kevent kev;
    EV_SET(&kev, socket->fd, EVFILT_READ, EV_ADD, 0, 0, 0);

    triggered.push_back(kevent());
}
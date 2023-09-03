#pragma once

#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <arpa/inet.h>
#include <netdb.h>

#include <memory>

#include <string>

#include <poll.h>
#include <thread>
#include <unordered_map>
#include <functional>

// #include <iostream>

#define KB 1024
#define MB 1024*1024

#define chk_bit(a,b) ((a&b) == b)

namespace net {
    struct socket {
        int fd = -1;
        int i = -1;

        enum events {
            CONNECT,
            DATA,
            DISCONNECT,
        };

        socket(int _fd);
        ~socket();

        std::vector<std::function<void()>> on_connect_handlers;
        std::vector<std::function<void(std::string)>> on_data_handlers;
        std::vector<std::function<void()>> on_disconnect_handlers;

        net::socket& on(events event, std::function<void()> handler);
        net::socket& on(events event, std::function<void(std::string)> handler);

        void close();

        size_t operator>>(std::string& string);
        size_t operator<<(const std::string string);
    };

    struct server {
        socket _socket;

        bool listening = false;
        std::thread * thread;
        // std::unordered_map<int, int> fd_to_socket; // fd -> index -> socket* 
        // server(int fd);

        enum events {
            CONNECT,
            LISTEN,
        };

        std::vector<std::function<void(net::socket&)>> on_connect_handlers;
        std::vector<std::function<void()>> on_listen_handlers;

        server(int fd);
        ~server();

        void listen_threaded(int backlog = 16);
        void listen_block(int backlog = 16);

        net::server& on(events event, std::function<void(net::socket&)> handler);
        net::server& on(events event, std::function<void()> handler);

        int& fd();
    };

    struct event_loop {

        bool running = false;
        // std::thread * _thread = NULL;
        std::thread thread;

        ~event_loop();

        void start(bool block=false);

        void add(std::shared_ptr<net::socket> socket);

        bool should_join = false;
        void join();

        std::vector<std::shared_ptr<net::socket>> sockets;
        std::vector<pollfd> pollfds;
    };


    std::unique_ptr<server> create_server(const char * path);
    std::unique_ptr<server> create_server(int port);


    std::unique_ptr<socket> connect(const char * address, const char * port);
    std::unique_ptr<socket> connect(const char * address, int port);
    std::unique_ptr<socket> connect(const char * path);

};

void net::socket::close() {
    if(fd != -1) {
        ::close(fd);
        fd = -1;
    }
}

int& net::server::fd() {
    return _socket.fd;
}


void default_on_end(net::socket& a);
void default_on_message(net::socket& a, std::string b) {}
void default_on_connect(net::server& a, net::socket& b) {}
void default_on_listen(net::server& a) {}
net::server::server(int _fd) : _socket(_fd) {
}

net::socket::socket(int _fd) : fd(_fd) {

}

net::server::~server() {

}

net::socket& net::socket::on(net::socket::events event, std::function<void()> handler) {
    switch(event) {
    case CONNECT:
        on_connect_handlers.push_back(handler);
        break;
    case DISCONNECT:
        on_disconnect_handlers.push_back(handler);
        break;
    default:
        throw std::runtime_error("\"CONNECT\" listener has the wrong type");
    }
    return *this;
}

net::socket& net::socket::on(net::socket::events event, std::function<void(std::string)> handler) {
    switch(event) {
    case DATA:
        on_data_handlers.push_back(handler);
        break;
    default:
        throw std::runtime_error("\"LISTEN\" listener has the wrong type");
    }
    return *this;
}

net::server& net::server::on(net::server::events event, std::function<void(socket&)> handler) {
    switch(event) {
    case CONNECT:
        on_connect_handlers.push_back(handler);
        break;
    default:
        throw std::runtime_error("\"CONNECT\" listener has the wrong type");
    }
    return *this;
}

net::server& net::server::on(net::server::events event, std::function<void()> handler) {
    switch(event) {
    case LISTEN:
        on_listen_handlers.push_back(handler);
        break;
    default:
        throw std::runtime_error("\"LISTEN\" listener has the wrong type");
    }
    return *this;
}

void listen_block(net::server* server, int& backlog) {
    int& server_fd = server->fd();

    int success = ::listen(server_fd, backlog);
    if(success < 0) {
        throw std::runtime_error("Couldn't listen.");
    }

    std::vector<pollfd> pollfds;
    pollfds.resize(1);
    pollfds[0].fd = server_fd;
    pollfds[0].events = POLLIN;

    server->listening = true;



    for(auto i = server->on_listen_handlers.begin(); i != server->on_listen_handlers.end(); i ++) {
        (*i)();
    }

    std::unordered_map<int, std::unique_ptr<net::socket>> connections;

    while(server->listening) {
        int n_ready = poll(&pollfds[0], pollfds.size(), -1);
        if(n_ready < 0) {
            throw std::runtime_error("Polling failed");
        }

        if(chk_bit(pollfds[0].revents, POLLIN)) {
            int client_fd = accept(server_fd, NULL, NULL);
            if(client_fd < 0) {
                throw std::runtime_error("Accept failed");
            }

            pollfd _pollfd;
            _pollfd.fd = client_fd;
            _pollfd.events = POLLIN;
            
            pollfds.push_back(_pollfd);

            std::unique_ptr<net::socket> client = std::make_unique<net::socket>(client_fd);
            
            for(auto i = server->on_connect_handlers.begin(); i != server->on_connect_handlers.end(); i ++) {
                (*i)(*client);
            }

            for(auto i = client->on_connect_handlers.begin(); i != client->on_connect_handlers.end(); i ++) {
                (*i)();
            }

            connections[client_fd] = std::move(client);
        }

        size_t n_closed_fds = 0;
        for(size_t i = 1; i < pollfds.size(); i ++) {
            if(chk_bit(pollfds[i].revents, POLLNVAL)) n_closed_fds++;

            if(chk_bit(pollfds[i].revents, POLLIN)) {
                int fd = pollfds[i].fd;

                std::unique_ptr<net::socket>& client = connections[fd];
                
                if(client->on_data_handlers.size() != 0) {
                    char buf[4096];
                    size_t n_bytes = recv(fd, buf, 4095, 0);
                    if(n_bytes == -1) {
                        throw std::runtime_error("Error with recv");
                    }
                    buf[n_bytes] = '\0';
                    
                    std::string data(buf);
                    
                    for(auto i = client->on_data_handlers.begin(); i != client->on_data_handlers.end(); i ++) {
                        (*i)(data);
                    }

                }
            }
            if(chk_bit(pollfds[i].revents, POLLHUP)) {
                // delete connections[pollfds[i].fd];
                connections.erase(pollfds[i].fd);
            }
        }


        if(n_closed_fds > pollfds.size() / 2) {
            for(size_t i = pollfds.size() - 1; i >= 1; i --) {
                if(chk_bit(pollfds[i].revents, POLLNVAL)) pollfds.erase(pollfds.begin() + i);
            }
        }

        // printf("------------------\n");
    }
}

void net::server::listen_threaded(int backlog) {
    // thread = new std::thread(listen_block, *this, backlog);
}


void net::server::listen_block(int backlog) {
    ::listen_block(this, backlog);
}

net::socket::~socket() {
    close();
}

size_t net::socket::operator<<(const std::string string) {
    size_t n_bytes = string.size();
    size_t s = send(fd, &string[0], n_bytes, 0);
    if(s == -1) {
        throw std::runtime_error("Something went wrong with send.");
    }
    return string.size() - n_bytes;
}

size_t net::socket::operator>>(std::string& string) {
    string = "";
    char buf[4096];
    
    pollfd _pollfd;
    _pollfd.fd = fd;
    _pollfd.events = POLLIN;

    int success = poll(&_pollfd, 1, -1);
    if(success < 0) {
        throw std::runtime_error("Poll failed");
    } 

    while(chk_bit(_pollfd.revents, POLLIN)) {
        
        size_t s = recv(fd, buf, 4095, 0);
        if(s == -1) {
            throw std::runtime_error("Recv failed");
        }

        buf[s] = '\0';
        
        string += buf;

        success = poll(&_pollfd, 1, 0);
        if(success == -1) {
            throw std::runtime_error("Poll failed");
        }
    }

    return string.size();
}


std::unique_ptr<net::server> net::create_server(const char * path) {
    sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path));

    int fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if(fd < 0) {
        throw std::runtime_error("Couldn't create a socket");
    }
    
    int success = ::bind(fd, (sockaddr *) &addr, sizeof(addr));
    if(success < 0) {
        throw std::runtime_error("Couldn't connect");
    }

    return std::make_unique<net::server>(fd);
}

std::unique_ptr<net::server> net::create_server(int port) {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0) {
        throw std::runtime_error("Couldn't create a socket");
    }


    int success = ::bind(fd, (sockaddr *) &addr, sizeof(addr));
    if(success < 0) {
        throw std::runtime_error("Couldn't bind");
    }
    
    return std::make_unique<net::server>(fd);
}

void* get_in_addr(sockaddr *s) {
    if(s->sa_family == AF_INET) return &((sockaddr_in *) s)->sin_addr;
    else return &((sockaddr_in6 *) s)->sin6_addr;
}

std::unique_ptr<net::socket> net::connect(const char * address, const char * port) {
    addrinfo hints;
    addrinfo *results;

    memset(&hints, 0, sizeof(hints)); 

    hints.ai_family = AF_INET;
    hints.ai_socktype= SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    int success = getaddrinfo(address, port, &hints, &results);

    int fd = -1;
    for(addrinfo * curr = results; curr != NULL; curr = curr->ai_next) {

        // char host[NI_MAXHOST];
        // char port[NI_MAXSERV];

        // if(getnameinfo(curr->ai_addr, curr->ai_addrlen, host, sizeof(host), port, sizeof(port), NI_NUMERICSERV) == 0) {
        //     char s[INET6_ADDRSTRLEN];
        //     inet_ntop(curr->ai_family, get_in_addr(curr->ai_addr), s, sizeof(s));
        //     printf("host: %s\nport: %s\nip: %s\n", host, port, s);
        // }

        fd = ::socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol);

        if(fd < 0) {
            close(fd);
            fd = -1;
            continue;
        }

        success = ::connect(fd, curr->ai_addr, curr->ai_addrlen);
        if(success >= 0) {
            break;
        }
        close(fd);
        fd = -1;
    }

    freeaddrinfo(results);

    if(fd < 0) {
        perror("coc");
        throw std::runtime_error("Couldn't connect");
    }

    return std::make_unique<net::socket>(fd);
}

std::unique_ptr<net::socket> net::connect(const char * addr, int port) {
    char buf[6];
    snprintf(buf, 6, "%d", port);
    return net::connect(addr, buf);
}

std::unique_ptr<net::socket> net::connect(const char * path) {
    sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path));

    int fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if(fd < 0) {
        throw std::runtime_error("Couldn't create a socket");
    }
    
    int success = ::connect(fd, (sockaddr *) &addr, sizeof(addr));
    if(success < 0) {
        throw std::runtime_error("Couldn't connect");
    }

    return std::make_unique<net::socket>(fd);
}


net::event_loop::~event_loop() {
    join();
}

// void recv_all(int fd, std::string& out) {
//     out = "";

//     pollfd _pollfd;
//     _pollfd.fd = fd;
//     _pollfd.events = POLLIN;

//     do {
//         char buf[2048];
//         size_t n = recv(fd, buf, 2047, 0);
//         buf[n] = '\0';

//         out += buf;

//         if(poll(&_pollfd, 1, 0) == -1) throw std::runtime_error("Poll failed");
//     } while(chk_bit(_pollfd.revents, POLLIN));
// }

size_t clean_up_dead_sockets(std::vector<std::shared_ptr<net::socket>>& sockets, std::vector<pollfd>& pollfds) {
    size_t i = 0;
    for(size_t j = 0; j < sockets.size(); j ++) {
        if(sockets[j]->fd != -1) {
            sockets[i] = sockets[j];
            pollfds[i] = pollfds[j];

            i++;
        }
    }
    sockets.resize(i);

    return i;
}

static void loop(net::event_loop* event_loop) {
    event_loop->running = true;
    event_loop->should_join = false;
    while(event_loop->running) {
        if(poll((&event_loop->pollfds[0]), event_loop->pollfds.size(), -1) == -1) {
            throw std::runtime_error("Poll failed");
        }

        size_t n_dead_sockets = 0;
        for(size_t i = 0; i < event_loop->pollfds.size(); i ++) {
            pollfd& curr_pollfd = event_loop->pollfds[i];
            net::socket& curr_socket = *(event_loop->sockets[i]);

            if(curr_socket.fd == -1) {
                n_dead_sockets++;
                continue; // Closed.
            }

            if(chk_bit(curr_pollfd.revents, POLLIN)) {
                std::string s = "";
                // recv_all(curr_pollfd.fd, s);
                char buf[32 * KB];
                size_t n_bytes = recv(curr_pollfd.fd,  buf, sizeof(buf) - 1, 0);
                buf[n_bytes] = '\0';

                s = buf;

                for(auto j = curr_socket.on_data_handlers.begin(); j != curr_socket.on_data_handlers.end(); j ++) {
                    (*j)(s);
                }
            }
            
            if(chk_bit(curr_pollfd.revents, POLLHUP)) {
                for(auto j = curr_socket.on_disconnect_handlers.begin(); j != curr_socket.on_disconnect_handlers.end(); j ++) {
                    (*j)();
                }
                curr_socket.close();
                n_dead_sockets++;
            }
        }

        size_t n_sockets = event_loop->sockets.size();
        if(n_dead_sockets >= n_sockets / 2) {
            clean_up_dead_sockets(event_loop->sockets, event_loop->pollfds);
        }

        if((event_loop->sockets.size() == 0)) {
            event_loop->running = false;
        }
    }
}

void net::event_loop::start(bool block) {
    if(block)
        thread = std::thread(loop, this);
    else
        loop(this);
}

void net::event_loop::add(std::shared_ptr<net::socket> socket) {
    pollfd new_pollfd;
    new_pollfd.fd = socket->fd;
    new_pollfd.events = POLLIN;

    sockets.push_back(socket);
    pollfds.push_back(new_pollfd);
}

void net::event_loop::join() {
    if(thread.joinable()) {
        should_join = true;
        thread.join();
    }
}
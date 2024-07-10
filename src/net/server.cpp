
#include <net/net.hpp>
#include <poll.h>
#include <stdexcept>

int& net::server::fd() {
    return _socket.fd;
}


net::server::server(int _fd) : _socket(_fd) {
}

net::server::~server() {

}

net::server& net::server::on(net::server::events event, net::server::on_connect_handler handler) {
    switch(event) {
    case CONNECT:
        handlers.on_connect.push_back(handler);
        break;
    default:
        throw std::runtime_error("\"CONNECT\" listener has the wrong type");
    }
    return *this;
}

net::server& net::server::on(net::server::events event, net::server::on_listen_handler handler) {
    switch(event) {
    case LISTEN:
        handlers.on_listen.push_back(handler);
        break;
    default:
        throw std::runtime_error("\"LISTEN\" listener has the wrong type");
    }
    return *this;
}

void net::server::listen(int backlog) {
    int& server_fd = fd();

    int success = ::listen(server_fd, backlog);
    if(success < 0) {
        throw std::runtime_error("Couldn't listen.");
    }
}



net::server net::create_server(const char * path) {
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

    return net::server(fd);
}

net::server net::create_server(int port) {
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
    
    return net::server(fd);
}


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




net::server::server(const net::server& other) : _socket(other._socket), _connections(other._connections), listening(other.listening) {

}

net::server::server(net::server&& other) : _socket(std::move(other._socket)), _connections(std::move(other._connections)), listening(other.listening) {
    other.listening = false;
}


net::server& net::server::operator=(const net::server& other) {
    _socket = other._socket;
    _connections = other._connections;
    listening = other.listening;
}

net::server& net::server::operator=(net::server&& other) {
    _socket = std::move(other._socket);
    _connections = std::move(other._connections);
    listening = other.listening;
    other.listening = false;
}

// void listen_block(net::server* server, int& backlog) {
//     int& server_fd = server->fd();

//     int success = ::listen(server_fd, backlog);
//     if(success < 0) {
//         throw std::runtime_error("Couldn't listen.");
//     }

//     std::vector<pollfd> pollfds;
//     pollfds.resize(1);
//     pollfds[0].fd = server_fd;
//     pollfds[0].events = POLLIN;

//     server->listening = true;



//     for(auto i = server->on_listen_handlers.begin(); i != server->on_listen_handlers.end(); i ++) {
//         (*i)();
//     }

//     std::unordered_map<int, std::unique_ptr<net::socket>> connections;

//     while(server->listening) {
//         int n_ready = poll(&pollfds[0], pollfds.size(), -1);
//         if(n_ready < 0) {
//             throw std::runtime_error("Polling failed");
//         }

//         if(chk_bit(pollfds[0].revents, POLLIN)) {
//             int client_fd = accept(server_fd, NULL, NULL);
//             if(client_fd < 0) {
//                 throw std::runtime_error("Accept failed");
//             }

//             pollfd _pollfd;
//             _pollfd.fd = client_fd;
//             _pollfd.events = POLLIN;
            
//             pollfds.push_back(_pollfd);

//             std::unique_ptr<net::socket> client = std::make_unique<net::socket>(client_fd);
            
//             for(auto i = server->on_connect_handlers.begin(); i != server->on_connect_handlers.end(); i ++) {
//                 (*i)(*client);
//             }

//             for(auto i = client->on_connect_handlers.begin(); i != client->on_connect_handlers.end(); i ++) {
//                 (*i)();
//             }

//             connections[client_fd] = std::move(client);
//         }

//         size_t n_closed_fds = 0;
//         for(size_t i = 1; i < pollfds.size(); i ++) {
//             if(chk_bit(pollfds[i].revents, POLLNVAL)) n_closed_fds++;

//             if(chk_bit(pollfds[i].revents, POLLIN)) {
//                 int fd = pollfds[i].fd;

//                 std::unique_ptr<net::socket>& client = connections[fd];
                
//                 if(client->on_data_handlers.size() != 0) {
//                     char buf[4096];
//                     size_t n_bytes = recv(fd, buf, 4095, 0);
//                     if(n_bytes == -1) {
//                         throw std::runtime_error("Error with recv");
//                     }
//                     buf[n_bytes] = '\0';
                    
//                     std::string data(buf);
                    
//                     for(auto i = client->on_data_handlers.begin(); i != client->on_data_handlers.end(); i ++) {
//                         (*i)(data);
//                     }

//                 }
//             }
//             if(chk_bit(pollfds[i].revents, POLLHUP)) {
//                 // delete connections[pollfds[i].fd];
//                 connections.erase(pollfds[i].fd);
//             }
//         }


//         if(n_closed_fds > pollfds.size() / 2) {
//             for(size_t i = pollfds.size() - 1; i >= 1; i --) {
//                 if(chk_bit(pollfds[i].revents, POLLNVAL)) pollfds.erase(pollfds.begin() + i);
//             }
//         }

//         // printf("------------------\n");
//     }
// }

// static void loop(net::event_loop* event_loop) {
//     event_loop->running = true;
//     event_loop->should_join = false;
//     while(event_loop->running) {
//         if(poll((&event_loop->pollfds[0]), event_loop->pollfds.size(), -1) == -1) {
//             throw std::runtime_error("Poll failed");
//         }

//         size_t n_dead_sockets = 0;
//         for(size_t i = 0; i < event_loop->pollfds.size(); i ++) {
//             pollfd& curr_pollfd = event_loop->pollfds[i];
//             net::socket& curr_socket = *(event_loop->sockets[i]);

//             if(curr_socket.fd == -1) {
//                 n_dead_sockets++;
//                 continue; // Closed.
//             }

//             if(chk_bit(curr_pollfd.revents, POLLIN)) {
//                 std::string s = "";
//                 // recv_all(curr_pollfd.fd, s);
//                 char buf[32 * KB];
//                 size_t n_bytes = recv(curr_pollfd.fd,  buf, sizeof(buf) - 1, 0);
//                 buf[n_bytes] = '\0';

//                 s = buf;

//                 for(auto j = curr_socket.on_data_handlers.begin(); j != curr_socket.on_data_handlers.end(); j ++) {
//                     (*j)(s);
//                 }
//             }
            
//             if(chk_bit(curr_pollfd.revents, POLLHUP)) {
//                 for(auto j = curr_socket.on_disconnect_handlers.begin(); j != curr_socket.on_disconnect_handlers.end(); j ++) {
//                     (*j)();
//                 }
//                 curr_socket.close();
//                 n_dead_sockets++;
//             }
//         }

//         size_t n_sockets = event_loop->sockets.size();
//         if(n_dead_sockets >= n_sockets / 2) {
//             clean_up_dead_sockets(event_loop->sockets, event_loop->pollfds);
//         }

//         if((event_loop->sockets.size() == 0)) {
//             event_loop->running = false;
//         }
//     }
// }

// void net::event_loop::start(bool block) {
//     if(block)
//         thread = std::thread(loop, this);
//     else
//         loop(this);
// }

// void net::event_loop::add(std::shared_ptr<net::socket> socket) {
//     pollfd new_pollfd;
//     new_pollfd.fd = socket->fd;
//     new_pollfd.events = POLLIN;

//     sockets.push_back(socket);
//     pollfds.push_back(new_pollfd);
// }

// void net::event_loop::join() {
//     if(thread.joinable()) {
//         should_join = true;
//         thread.join();
//     }
// }
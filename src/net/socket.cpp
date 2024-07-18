#include <net/socket.hpp>
#include <net/net.hpp>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <unistd.h>
#include <poll.h>
#include <stdexcept>

#define chk_bit(a,b) ((a&b) == b)

void net::socket::close() {
    if(_fd != -1) {
        ::close(_fd);
        _fd = -1;
    }
}
net::socket::socket(int fd) : _fd(fd) {
    opened = _fd != -1;
}

net::socket& net::socket::on(net::socket::events event, std::function<void()> handler) {
    switch(event) {
    case CONNECT:
        handlers.on_connect.add(handler);
        break;
    case DISCONNECT:
        handlers.on_disconnect.add(handler);
        break;
    default:
        throw std::runtime_error("\"CONNECT\" listener has the wrong type");
    }
    return *this;
}

net::socket& net::socket::on(net::socket::events event, stream::readable::on_data_handler handler) {
    switch(event) {
    case DATA:
        stream::readable::handlers.on_data.add(handler);
        break;
    default:
        throw std::runtime_error("\"LISTEN\" listener has the wrong type");
    }
    return *this;
}

net::socket::~socket() {
    _destroy_buffers();
    close();
}

size_t net::socket::operator<<(const std::string string) {
    size_t n_bytes = string.size();
    ssize_t s = send(_fd, &string[0], n_bytes, 0);
    if(s == -1) {
        throw std::runtime_error("Something went wrong with send.");
    }
    return string.size() - n_bytes;
}

size_t net::socket::operator>>(std::string& string) {
    string = "";
    char buf[4096];

    ssize_t s = recv(_fd, buf, 4095, 0);
    if(s == -1) {
        perror("huh?");
        throw std::runtime_error("Recv failed");
    }
    
    buf[s] = '\0';
    string = std::string(buf);

    return s;
}

net::socket::socket(const net::socket& other) {
    if(other._fd != -1) {
        _fd = dup(other._fd);
        if(_fd == -1) throw std::runtime_error("dup() failed.");
    }
}

net::socket::socket(net::socket&& other) {
    _fd = other._fd;
    other._fd = -1;
}

net::socket& net::socket::operator=(net::socket&& other) {
    _fd = other._fd;
    other._fd = -1;
    return *this;
}
net::socket& net::socket::operator=(const net::socket& other) {

    _fd = dup(other._fd);
    if(_fd == -1) throw std::runtime_error("dup() failed.");
    
    return *this;
}


// template<>
// void net::socket::on<events::CONNECT>(std::function<void()> f) {
//     handlers.on_connect.push_back(f);
// }
// template<>
// void net::socket::on<events::DATA>(std::function<void(std::string)>> f) {
//     handlers.on_data.push_back(f);
// }
// template<>
// void net::socket::on<events::DISCONNECT>(std::function<void()> f) {
//     handlers.on_disconnect.push_back(f);
// }



// template<>
// void net::socket_on<net::socket::CONNECT>(net::socket& s, std::function<void()> f) {
//     s.handlers.on_connect.push_back(f);
// }
// template<>
// void net::socket_on<net::socket::DATA>(net::socket& s, std::function<void(std::string)>> f) {

// }
// template<>
// void net::socket_on<net::socket::DISCONNECT>(net::socket& s, std::function<void()> f) {

// }

callback_list<net::socket::on_connect_handler>::callback_manager net::socket::on_connect(net::socket::on_connect_handler f) {
    return handlers.on_connect.add(f);
}

// callback<std::function<void(std::string)>> net::socket::on_data(std::function<void(std::string)> f) {
//     handlers.on_data.push_back(f);
//     return callback<std::function<void(std::string)>>(handlers.on_data, handlers.on_data.end()--);
// }

callback_list<net::socket::on_disconnect_handler>::callback_manager net::socket::on_disconnect(net::socket::on_disconnect_handler f) {
    return handlers.on_disconnect.add(f);
}

void net::socket::_initialize_buffers() {
    _send_buffer = new char[_send_buffer_size];
    _send_buffer_i = 0;
}

void net::socket::_destroy_buffers() {
    if(_send_buffer != NULL) {
        delete _send_buffer;
        _send_buffer = NULL;
    }
}

bool net::socket::_write(std::span<const char> data) {
    for(auto c : data) {
        _send_buffer[_send_buffer_i++] = c;
        if(_send_buffer_i == _send_buffer_size) {
            _drain();
        }
    }
    return _drain();
}

bool net::socket::_drain() {
    ssize_t s = send(_fd, _send_buffer, _send_buffer_i, 0);
    _send_buffer_i = 0;
    if(s == -1) {
        throw std::runtime_error("Something went wrong with send.");
    }
    return true;
}



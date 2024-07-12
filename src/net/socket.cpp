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
    if(fd != -1) {
        ::close(fd);
        fd = -1;
    }
}
net::socket::socket(int _fd) : fd(_fd) {

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

    ssize_t s = recv(fd, buf, 4095, 0);
    if(s == -1) {
        perror("huh?");
        throw std::runtime_error("Recv failed");
    }
    
    buf[s] = '\0';
    string = std::string(buf);

    return s;
}

net::socket::socket(const net::socket& other) {
    fd = dup(other.fd);
    if(fd == -1) throw std::runtime_error("dup() failed.");
}

net::socket::socket(net::socket&& other) {
    fd = other.fd;
    other.fd = -1;
}

net::socket& net::socket::operator=(net::socket&& other) {
    fd = other.fd;
    other.fd = -1;
    return *this;
}
net::socket& net::socket::operator=(const net::socket& other) {

    fd = dup(other.fd);
    if(fd == -1) throw std::runtime_error("dup() failed.");
    
    return *this;
}
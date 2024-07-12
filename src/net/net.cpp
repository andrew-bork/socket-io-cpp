
#include "net/socket.hpp"
#include "net/net.hpp"

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <unistd.h>
#include <poll.h>
#include <stdexcept>

#include <cstring>


void* get_in_addr(sockaddr *s) {
    if(s->sa_family == AF_INET) return &((sockaddr_in *) s)->sin_addr;
    else return &((sockaddr_in6 *) s)->sin6_addr;
}




net::socket net::connect(const char * address, const char * port) {
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

    return net::socket(fd);
}

net::socket net::connect(const char * addr, int port) {
    char buf[6];
    snprintf(buf, 6, "%d", port);
    return net::connect(addr, buf);
}

net::socket net::connect(const char * path) {
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

    return net::socket(fd);
}


// net::event_loop::~event_loop() {
//     join();
// }

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

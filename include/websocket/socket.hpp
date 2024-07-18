#pragma once
#include <string>
#include <list>
#include <functional>
#include "websocket/enums.hpp"
#include "net/socket.hpp"
#include "url/url.hpp"
#include "http/parser.hpp"
#include <openssl/evp.h>
namespace websocket {
    struct socket {

        typedef std::function<void(void)> on_open_handler;
        typedef std::function<void(std::span<const char>)> on_message_handler;

        // typedef std::function<void(void)> ;

        socket(const net::socket& socket, std::string path="/");
        socket(net::socket&& socket, std::string path="/");
        socket(std::string url);
        socket(url::url _url);

        ~socket();
        

        void send(const std::string& data);

        void initiate_client_handshake();

        void close();

        void set_host(std::string host);


        struct {
            callback_list<on_open_handler> on_open;
            callback_list<on_message_handler> on_message;
        } handlers;

        inline callback_list<on_open_handler>::callback_manager on_open(on_open_handler f) {
            return handlers.on_open.add(f);
        }
        inline callback_list<on_message_handler>::callback_manager on_message(on_message_handler f) {
            return handlers.on_message.add(f);
        }

        net::socket& connection();

        private: 
            url::url _url;
            net::socket _socket;

            struct handshake_manager {
                websocket::socket& _socket;
                callback_list<net::socket::on_connect_handler>::callback_manager _sock_connect_handler;
                callback_list<stream::readable::on_data_handler>::callback_manager _sock_data_handler;
                std::string _secure_key;
                http::response_parser _response_parser;

                EVP_MD_CTX *_hasher_ctx = NULL;

                handshake_manager(websocket::socket& socket);
                ~handshake_manager();
            };

            handshake_manager* _handshake = NULL;
    };


};
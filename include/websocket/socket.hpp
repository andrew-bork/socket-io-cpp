#pragma once
#include <string>
#include <list>
#include <functional>
#include "websocket/enums.hpp"
#include "net/socket.hpp"
#include "url/url.hpp"
#include "http/parser.hpp"
// #include <openssl/evp.h>

// #include "util/hash.hpp"


namespace websocket {
    struct socket {

        typedef std::function<void(void)> on_open_handler;
        typedef std::function<void(int, std::span<const char>)> on_close_handler;
        typedef std::function<void(std::span<const char>)> on_pong_handler;
        typedef std::function<void(std::span<const char>)> on_message_handler;

        // typedef std::function<void(void)> ;

        socket(const net::socket& socket, std::string path="/");
        socket(net::socket&& socket, std::string path="/");
        socket(std::string url);
        socket(url::url _url);

        ~socket();
        

        void send(std::span<const char> data);

        void initiate_client_handshake();

        void close();

        void set_host(std::string host);

        void ping(std::span<const char> data);

        struct {
            callback_list<on_open_handler> on_open;
            callback_list<on_message_handler> on_message;
            callback_list<on_close_handler> on_close;
            callback_list<on_pong_handler> on_pong;
        } handlers;

        inline callback_list<on_open_handler>::callback_manager on_open(on_open_handler f) {
            return handlers.on_open.add(f);
        }
        inline callback_list<on_message_handler>::callback_manager on_message(on_message_handler f) {
            return handlers.on_message.add(f);
        }

        inline callback_list<on_close_handler>::callback_manager on_close(on_close_handler f) {
            return handlers.on_close.add(f);
        }

        inline callback_list<on_pong_handler>::callback_manager on_pong(on_pong_handler f) {
            return handlers.on_pong.add(f);
        }

        net::socket& connection();

        private: 
            void _initialize();
            void _pong(std::span<const char> data);
            void _send(std::span<const char> data, bool finish = false, bool masked = false, frame_options opcode = TEXT);

            url::url _url;
            net::socket _socket;

            bool _masked = true;

            // size_t _send_buffer_max_size = 4096;
            // size_t _recieve_buffer_max_size = 4096;
            std::vector<char> _send_buffer;
            std::vector<char> _recieve_buffer;

            callback_list<net::socket::on_data_handler>::callback_manager _sock_data_handler;

            struct handshake_manager {
                websocket::socket& _socket;
                // callback_list<net::socket::on_connect_handler>::callback_manager _sock_connect_handler;
                callback_list<stream::readable::on_data_handler>::callback_manager _sock_data_handler;
                std::string _secure_key, _secure_key_accept;
                http::response_parser _response_parser;

                // EVP_MD_CTX *_hasher_ctx = NULL;
                // hash::sha1 _hasher;

                handshake_manager(websocket::socket& socket);
                ~handshake_manager();
            };

            struct frame_parser {
                enum {
                    OPCODE,

                    PAYLOAD_LENGTH_1,
                    PAYLOAD_LENGTH_2,
                    PAYLOAD_LENGTH_3,
                    PAYLOAD_LENGTH_4,
                    PAYLOAD_LENGTH_5,
                    PAYLOAD_LENGTH_6,
                    PAYLOAD_LENGTH_7,
                    PAYLOAD_LENGTH_8,
                    PAYLOAD_LENGTH_9,

                    MASKING_KEY_1,
                    MASKING_KEY_2,
                    MASKING_KEY_3,
                    MASKING_KEY_4,

                    PAYLOAD,
                    DONE,

                } state;
                bool finish = false;
                unsigned char opcode = 0;

                size_t payload_length, i;

                bool masked = false;
                int mask = 0;

                std::vector<char> payload;
                
                std::function<void(std::span<const char>)> on_close = [](std::span<const char>){};
                std::function<void(std::span<const char>)> on_ping = [](std::span<const char>){};
                std::function<void(std::span<const char>)> on_pong = [](std::span<const char>){};
                std::function<void(std::span<const char>)> on_data_frame = [](std::span<const char>){};

                bool parse(std::span<const char> data);
            };

            frame_parser _frame_parser;

            handshake_manager* _handshake = NULL;
    };


};
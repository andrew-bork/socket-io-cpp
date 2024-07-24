#pragma once


namespace websocket {
    enum events {
        CONNECT,
        DISCONNECT,
        DATA,
        MESSAGE,
    };

    enum frame_options : unsigned char{
        FIN             = 0b10000000, 
        CONT            = 0b00000000, 
        FIN_CONT_MASK   = 0b10000000,

        TEXT            = 0b00000001, 
        BIN             = 0b00000010, 
        CLOSE           = 0b00000100, 
        PING            = 0b00000101, 
        PONG            = 0b00000110,
        OPCODE_MASK     = 0b00000111,
    };
};
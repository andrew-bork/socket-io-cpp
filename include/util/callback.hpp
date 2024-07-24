#pragma once

#include <list>

template<typename T>
struct callback_list {
    struct callback {
        T function;
        bool once = false;
        bool removed = false;
        // std::list<T>* list = NULL;
        callback(T _f) : function(_f) {}
    };

    struct callback_manager {
        typedef typename std::list<callback>::iterator iterator;
        iterator curr;

        bool removed = false;

        callback_manager() {
            removed = true;
        }
        callback_manager(iterator _curr) : curr(_curr) {}

        void remove() {
            if(!removed) {
                (*curr).removed = true;
                removed = true;
            }
        }
    };

    std::list<callback> handlers;

    callback_manager add(T function) {
        handlers.emplace_front(function);
        return callback_manager(handlers.begin());
    }

    template<class... Ts>
    void call(Ts... args) {
        for(auto i = handlers.begin(); i != handlers.end(); i ++) {
            if((*i).removed) {
                auto j = i;
                i--;
                handlers.erase(j);
                continue;
            }
            (*i).function(args...);
            if((*i).once) {
                auto j = i;
                i--;
                handlers.erase(j);
                continue;
            }
        }
    }
};

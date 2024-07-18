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

        callback_manager() {}
        callback_manager(iterator _curr) : curr(_curr) {}

        void remove() {
            (*curr).removed = true;
        }
    };

    std::list<callback> handlers;

    callback_manager add(T function) {
        handlers.emplace_back(function);
        return callback_manager(--handlers.end());
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

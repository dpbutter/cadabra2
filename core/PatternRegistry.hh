// Registry to special patterns used to simplify/shortcut pattern matches
#pragma once

#include "Storage.hh"
#include "Compare.hh"

namespace cadabra {

    class PatternRegistry {
        public:
            size_t size() {
                return registry_.size();
            }
            void clear() {
                registry_.clear();
            }

        private:
            std::set<Ex, Ex_is_less> registry_;
    };

}
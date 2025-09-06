#pragma once

#include "Props.hh"
#include "Compare.hh"
#include "Storage.hh"

namespace cadabra {

    class Tracker {
        private:
            Properties* props;
			std::map<patternId_t, std::unordered_set<Ex::node_t*>> map_;

        public:
            Tracker(Properties* p) : props(p) {}
            
            void track_node(Ex::node_t* node) {
                if (auto id = std::get_if<patternId_t>( &(node->data.content)) ) {
                    map_[*id].insert(node);
                }
            }

            // FIXME: const?
            std::unordered_set<Ex::node_t*>* get_nodes(patternId_t id) {
                auto it = map_.find(id);
                if (it == map_.end()) {
                    return nullptr;
                } else {
                    return &(it->second);
                }
            }

    };
}

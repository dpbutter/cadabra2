#pragma once

#include "Props.hh"
#include "Compare.hh"

namespace cadabra {

    class PatternTracker {
        public:
            PatternTracker(Properties* p) : props(p) {}

            struct Hash {
                Properties* p;
                Hash(Properties* props) : p(props) {}
                size_t operator()(Ex::iterator it) {
                    return Ex::calc_hash(it, *p);
                }
            };

            struct Equal {
                Properties* p;
                Equal(Properties* props) : p(props) {}
                bool operator()(Ex::iterator it1, Ex::iterator it2) {
                    static Ex_comparator comp(*p);
                    comp.clear();
                    auto ret = comp.equal_subtree(it1, it2);
                    switch (ret) {
                        case Ex_comparator::match_t::subtree_match:
                        case Ex_comparator::match_t::match_index_less:
                        case Ex_comparator::match_t::match_index_greater:
                            return true;
                        default:
                            return false;
                    }
                }
            };

//            std::unordered_map<Ex::pre_order_iterator, const pattern*, Hash, Equal> tracked_patterns;

        private:
            Properties* props;
    };
}

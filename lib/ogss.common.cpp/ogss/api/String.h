//
// Created by Timm Felden on 06.11.15.
//

#ifndef SKILL_CPP_COMMON_STRING_H
#define SKILL_CPP_COMMON_STRING_H

#include <string>
#include <functional>
#include "../common.h"

namespace ogss {
    namespace api {

        /**
         * we will be using unique pointers to string objects as strings
         */
        typedef const std::string *String;

        using ogss::api::String;

        /**
         * OGSS-comparison of strings
         */
        struct ogssLess {
            bool operator()(const String &lhs, const String &rhs) const {
                if (nullptr == lhs)
                    return true;
                if (nullptr == rhs)
                    return false;

                const long cmp = lhs->size() - rhs->size();
                return cmp == 0 ? *lhs < *rhs : cmp < 0;
            }
        };

        /**
         * image equality based comparison of strings
         */
        struct equalityLess {
            bool operator()(const String &lhs, const String &rhs) const {
                return *lhs < *rhs;
            }
        };

        /**
         * image equality based hashcode of strings
         */
        struct equalityHash {
            size_t operator()(const String &s) const noexcept {
                std::hash<std::string> hash;
                return hash(*s);
            }
        };

        /**
         * image equality based comparison of strings
         */
        struct equalityEquals {
            bool operator()(const String &x, const String &y) const {
                std::equal_to<std::string> eq;
                return eq(*x, *y);
            }
        };
    }
}


#endif //SKILL_CPP_COMMON_STRING_H

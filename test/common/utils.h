//
// Created by Timm Felden on 05.02.16.
//

#ifndef OGSS_CPP_TESTSUITE_UTILS_H
#define OGSS_CPP_TESTSUITE_UTILS_H

#include <ogss/api/File.h>
#include <ogss/api/Arrays.h>
#include <ogss/api/Sets.h>
#include <ogss/api/Maps.h>

namespace common {
    template<typename sf>
    std::unique_ptr<sf> tempFile(ogss::api::WriteMode mode = ::ogss::api::WriteMode::write) {
        return std::unique_ptr<sf>(sf::open(std::string(tmpnam(nullptr)), mode | ogss::api::ReadMode::create));
    }

    /**
     * create an array; used by test generation
     *
     * @note the likely memory leak is an acceptable issue
     */
    template<typename T>
    ogss::api::Array<T> *array() {
        return new ogss::api::Array<T>();
    }

    /**
     * create a set; used by test generation
     *
     * @note the likely memory leak is an acceptable issue
     */
    template<typename T>
    ogss::api::Set<T> *set() {
        return new ogss::api::Set<T>();
    }

    /**
     * create a map; used by test generation
     *
     * @note the likely memory leak is an acceptable issue
     */
    template<typename K, typename V>
    ogss::api::Map<K,V> *map() {
        return new ogss::api::Map<K,V>();
    }

    /**
     * put values into an array
     */
    template<typename T>
    ogss::api::Array<T> *put(ogss::api::Array<T> *arr, T v) {
        arr->push_back(v);
        return arr;
    }

    /**
     * put values into a set
     */
    template<typename T>
    ogss::api::Set<T> *put(ogss::api::Set<T> *arr, T v) {
        arr->insert(v);
        return arr;
    }

    /**
     * put values into a map
     */
    template<typename K, typename V>
    ogss::api::Map<K,V> *put(ogss::api::Map<K,V> *m, K k, V v) {
        (*m)[k] = v;
        return m;
    }
}

#endif //OGSS_CPP_TESTSUITE_UTILS_H

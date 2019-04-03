//
// Created by Timm Felden on 04.11.15.
//

#include <assert.h>
#include "StringPool.h"

using namespace ogss;
using api::String;

internal::StringPool::StringPool(streams::FileInputStream *in)
        : StringAccess(9), in(in), knownStrings(), idMap(), stringPositions(), lastID(0) {
    // ensure existence of fake entry
    stringPositions.push_back(std::pair<long, int>(-1L, -1));
    idMap.push_back(nullptr);
}

internal::StringPool::~StringPool() {
    for (auto s : knownStrings)
        if (literals.find(s) == literals.end())
            delete s;
}

String internal::StringPool::add(const char *target) {
    String result = new std::string(target);
    auto it = knownStrings.find(result);
    if (it != knownStrings.end()) {
        delete result;
        return *it;
    } else {
        knownStrings.insert(result);
        return result;
    }
}

String internal::StringPool::add(const char *target, int length) {
    String result = new std::string(target, length);
    auto it = knownStrings.find(result);
    if (it != knownStrings.end()) {
        delete result;
        return *it;
    } else {
        knownStrings.insert(result);
        return result;
    }
}

void internal::StringPool::addLiteral(String target) {
    // note to self: if this fails, we have to insert pb.name before advancing in the file!
    // note to self2: this can still fail, if we read an unknown type with the name of a known field
    assert(knownStrings.find(target) == knownStrings.end());

    knownStrings.insert(target);
    literals.insert(target);
}

//void internal::StringPool::prepareAndWrite(ogss::streams::FileOutputStream *out) {
//    SK_TODO;
////    // Insert new strings to the map;
////    // this is where duplications with lazy strings will be detected and eliminated
////    for (auto s : knownStrings) {
////        if (-1 == s->id) {
////            const_cast<api::string_t *>(s)->id = (ObjectID) idMap.size();
////            idMap.push_back(s);
////        }
////    }
////
////    const int64_t count = idMap.size() - 1;
////    // write block, if nonempty
////    if (count) {
////        out->v64(count);
////        for (int i = 1; i <= count; i++) {
////            out->v64(idMap[i]->length());
////        }
////        for (int i = 1; i <= count; i++) {
////            out->put(idMap[i]);
////        }
////    } else {
////        out->i8(0);
////    }
//}
//
//void internal::StringPool::prepareSerialization() {
//    // ensure all strings are present
//    for (auto i = stringPositions.size() - 1; i != 0; i--)
//        get(static_cast<ObjectID>(i));
//}

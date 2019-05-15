//
// Created by Timm Felden on 04.11.15.
//

#include <assert.h>

#include "StringPool.h"
#include "AbstractStringKeeper.h"

using namespace ogss;
using api::String;

internal::StringPool::StringPool(const AbstractStringKeeper *sk)
        : HullType(9, -1), in(nullptr), knownStrings(), literals(sk),
          literalStrings(sk->strings), literalStringCount(sk->size),
          positions(nullptr), lastID(0) {

    knownStrings.reserve(sk->size);
    for (ObjectID i = 0; i < sk->size; i++)
        knownStrings.insert(sk->strings[i]);
}

internal::StringPool::~StringPool() {
    // remove literals from knownStrings
    for (ObjectID i = 0; i < literals->size; i++)
        knownStrings.erase(literals->strings[i]);

    // delete remaining knownStrings
    for (auto s : knownStrings)
        delete s;

    delete[] positions;

    // free string literals, if they are dynamically heap allocated
    if (literalStrings != literals->strings)
        delete[] literalStrings;
}

void internal::StringPool::loadLazyData() {
    if (!in)
        return;

    const ObjectID last = idMap.size();
    for (ObjectID i = 1; i < last; i++) {
        if (!idMap[i])
            byID(i);
    }

    delete in;
    in = nullptr;
}


void internal::StringPool::readSL(ogss::streams::FileInputStream *in) {
    const int count = in->v32();
    if (0 == count) {
        // trivial merge
        return;
    }

    // known/file literal index
    int ki = 0, fi = 0;
    String next = in->literalString();

    // merge literals from file into literals
    std::vector<String> merged;
    merged.reserve(count);
    bool hasFI, hasKI;
    while ((hasFI = fi < count) | (hasKI = ki < literals->size)) {
        // note: we will intern the string only if it is unknown
        const int cmp = hasFI ? (hasKI ? ogssLess::javaCMP(next, literals->strings[ki]) : 1) : -1;

        if (0 <= cmp) {
            if (0 == cmp) {
                // discard next
                delete next;
                next = literals->strings[ki++];
            }
            // else, use next
            merged.push_back(next);
            idMap.push_back((void *) next);

            // in C++ we have to add literals to known strings, because there is no intern
            knownStrings.insert(next);

            if (++fi < count)
                next = in->literalString();
        } else {
            merged.push_back(literals->strings[ki++]);

            // in C++ we have to add literals to known strings, because there is no intern
            knownStrings.insert(next);
        }
    }

    // update literals if required
    if (literals->size != (literalStringCount = merged.size())) {
        // copy merged
        auto arr = new String[literalStringCount];
        int i = 0;
        for (String s : merged)
            arr[i++] = s;

        literalStrings = arr;
    }
    // else, we can reuse SK (set by constructor to ensure defined behaviour)

    lastID = literalStringCount;
}

ogss::BlockID internal::StringPool::allocateInstances(int count, ogss::streams::MappedInStream *in) {
    this->in = in;
    lastID += count;

    // read offsets
    auto offsets = new int[count];
    for (int i = 0; i < count; i++) {
        offsets[i] = in->v32();
    }

    // create positions
    int spi = idMap.size();
    const auto sp = new uint64_t[spi + count];
    positions = sp;

    // store offsets
    // @note this has to be done after reading all offsets, as sizes are relative to that point and decoding
    // is done using absolute sizes
    size_t last = in->getPosition();
    for (int i = 0; i < count; i++) {
        const size_t len = offsets[i];
        sp[spi++] = (last << 32LU) | len;
        idMap.push_back(nullptr);
        last += len;
    }

    delete[] offsets;

    return 0;
}

void internal::StringPool::read(BlockID block, streams::MappedInStream *in) {
    // -- done -- (strings are lazy)
}


void internal::StringPool::writeLiterals(StringPool *const sp, ogss::streams::FileOutputStream *out) {
    const int count = sp->hullOffset - 1;

    out->v64(count);
    for (int i = 1; i <= count; i++) {
        auto s = (String) sp->idMap[i];
        out->v64((int) s->size());
        out->put(s);
    }
}

bool internal::StringPool::write(ogss::streams::BufferedOutStream *out) {
    const int count = idMap.size() - hullOffset;
    if (0 == count)
        return true;

    out->v64(count);

    // lengths
    for (int i = 0; i < count; i++) {
        const String s = static_cast<String const>(idMap[i + hullOffset]);
        out->v64((int) s->size());
    }

    // data
    for (int i = 0; i < count; i++) {
        const String s = static_cast<String const>(idMap[i + hullOffset]);
        out->put(reinterpret_cast<const uint8_t *>(s->c_str()), s->size());
    }

    return false;
}
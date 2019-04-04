//
// Created by Timm Felden on 04.11.15.
//

#include <assert.h>
#include "StringPool.h"

using namespace ogss;
using api::String;

internal::StringPool::StringPool(streams::FileInputStream *in)
        : StringAccess(9), in(in), knownStrings(), idMap(), positions(nullptr), lastID(0) {
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


size_t internal::StringPool::S(int count, ogss::streams::InStream *in) {
    if (0 == count)
        return in->getPosition();

    // read offsets
    int *offsets = new int[count];
    for (int i = 0; i < count; i++) {
        offsets[i] = in->v32();
    }

    // resize string positions
    int spi;
    if (!positions) {
        positions = new uint64_t[1 + count];
        positions[0] = -1L;
        spi = 1;
    } else {
        spi = idMap.size();
        auto sp = new uint64_t[spi + count];
        std::memcpy(sp, positions, spi * sizeof(uint64_t));
        delete[] positions;
        positions = sp;
    }

    // store offsets
    // @note this has to be done after reading all offsets, as sizes are relative to that point and decoding
    // is done using absolute sizes
    size_t last = in->getPosition(), len;
    for (int i = 0; i < count; i++) {
        len = offsets[i];
        positions[spi++] = (((uint64_t) last) << 32UL) | len;
        last += len;
    }
    idMap.insert(idMap.cend(), (size_t) count, nullptr);
    lastID += count;

    delete[] offsets;
    return last;
}

void internal::StringPool::read() {
    // TODO broken architecture?
    throw std::logic_error("dead_code");
}

bool internal::StringPool::write(ogss::streams::BufferedOutStream &out) {
    const int count = idMap.size() - hullOffset;
    if (0 == count)
        return true;

    out.v64(count);

    // lengths
    for (int i = 0; i < count; i++) {
        out.v64((int) idMap[i + hullOffset]->size());
    }

    // data
    for (int i = 0; i < count; i++) {
        const String s = idMap[i + hullOffset];
        out.put(reinterpret_cast<const uint8_t *>(s->c_str()), s->size());
    }

    return false;
}

void internal::StringPool::allocateInstances(int count, ogss::streams::MappedInStream *in) {
    S(count, in);
}
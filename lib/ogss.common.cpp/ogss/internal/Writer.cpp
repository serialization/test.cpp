//
// Created by Timm Felden on 05.04.19.
//

#include <future>

#include "../api/File.h"
#include "../streams/FileOutputStream.h"
#include "Writer.h"

using namespace ogss::internal;

Writer::Writer(api::File *state, streams::FileOutputStream &out)
        : state(state) {
    /**
     * *************** * G * ****************
     */

    if (state->guard.empty()) {
        out.i16((short) 0x2622);
    } else {
        out.i8('#');
        out.put(&state->guard);
        out.i8(0);
    }

    /**
     * *************** * S * ****************
     */

    // prepare string pool
    {
        StringPool *const sp = (StringPool *) state->strings;
        sp->resetIDs();

        // create inverse map
        const int count = sp->knownStrings.size();
        sp->idMap.reserve(count);
        sp->IDs.reserve(count);
        for (String s : sp->knownStrings) {
            sp->IDs[s] = sp->idMap.size();
            sp->idMap.push_back(s);
        }
    }
    // write in parallel to writing of TF
    auto SB = std::async(std::launch::async,
                         StringPool::writeLiterals, (StringPool *) state->strings, &out);

    SB.get();
}
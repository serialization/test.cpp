//
// Created by Timm Felden on 03.04.19.
//

#include "SeqParser.h"

using namespace ogss::internal;

SeqParser::SeqParser(const std::string &path, streams::FileInputStream *in, const PoolBuilder &pb)
        : Parser(path, in, pb) {
}

void SeqParser::typeBlock() {


    /**
     * *************** * T Class * ****************
     */
    typeDefinitions();

    // calculate cached size and next for all pools
    {
        int cs = classes.size();
        if (0 != cs) {
            int i = cs - 2;
            if (i >= 0) {
                AbstractPool *n, *p = classes[i + 1];
                // propagate information in reverse order
                // i is the pool where next is set, hence we skip the last pool
                do {
                    n = p;
                    p = classes[i];

                    // by compactness, if n has a super pool, p is the previous pool
                    if (n->super) {
                        n->super->cachedSize += n->cachedSize;
                    }

                } while (--i >= 0);
            }

            // allocate data and start instance allocation jobs
            // note: this is different from Java, because we used templates in C++
            while (++i < cs) {
                AbstractPool *p = classes[i];
                p->allocateData();
                if (0 != p->staticDataInstances) {
                    p->allocateInstances();
                }
            }
        }
    }

    /**
     * *************** * T Container * ****************
     */
    TContainer();

    /**
     * *************** * T Enum * ****************
     */
    TEnum();

    /**
     * *************** * F * ****************
     */
    for (AbstractPool *p : classes) {
        readFields(p);
    }
}

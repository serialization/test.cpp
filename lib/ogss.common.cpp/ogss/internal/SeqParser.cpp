//
// Created by Timm Felden on 03.04.19.
//

#include "SeqParser.h"
#include "LazyField.h"

using namespace ogss::internal;

namespace ogss {
    namespace internal {
        class SeqReadTask final : public RTTIBase {
            DataField *const f;

            streams::MappedInStream *const map;

        public:
            SeqReadTask(DataField *f, streams::MappedInStream *in)
                    : f(f), map(in) {
            }

            ~SeqReadTask() final {
                delete map;
            }

            void run() {
                AbstractPool *const owner = f->owner;
                const int bpo = owner->bpo;
                const int end = bpo + owner->cachedSize;

                if (map->eof()) {
                    // TODO default initialization; this is a nop for now in Java
                } else {
                    f->read(bpo, end, *map);
                }

                if (!map->eof() && !(dynamic_cast<LazyField *>(f)))
                    throw std::out_of_range("read task did not consume InStream");
            }
        };
    }
}


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
                p->lastID = p->bpo + p->cachedSize;
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

void SeqParser::processData() {
    // we expect one HD-entry per field
    const int jobCount = fields.size();
    int remaining = jobCount;
    std::unique_ptr<RTTIBase *[]> jobs(new RTTIBase *[remaining]);

    while ((--remaining >= 0) & !in->eof()) {
        // create the map directly and use it for subsequent read-operations to avoid costly position and size
        // readjustments
        streams::MappedInStream *const map = in->jumpAndMap(in->v32() + 2);

        const int id = map->v32();
        RTTIBase *const f = fields.at(id);
        // overwrite entry to prevent duplicate read of the same field
        fields[id] = nullptr;

        if (auto p = dynamic_cast<HullType *>(f)) {
            const int count = map->v32();

            // start hull allocation job
            p->allocateInstances(count, map);

            // create hull read data task except for StringPool which is still lazy per element and eager per offset
            if (8 != p->typeID) {
                jobs.get()[id] = p;
            } else {
                jobs.get()[id] = nullptr;
            }

        } else if (auto fd = dynamic_cast<DataField *>(f)) {
            // create job with adjusted size that corresponds to the * in the specification (i.e. exactly the data)
            auto task = new SeqReadTask(fd, map);
            jobs.get()[id] = task;
        } else {
            delete map;
            ParseException(in.get(), "created the same read job twice");
        }
    }

    // perform read tasks
    for (int i = 0; i < jobCount; i++) {
        // only fields with overwritten IDs contain jobs
        if (nullptr == fields[i]) {
            RTTIBase *j = jobs[i];
            if (auto rt = dynamic_cast<SeqReadTask *>(j)) {
                rt->run();
                delete rt;
            } else if (auto ht = dynamic_cast<HullType *>(j)) {
                ht->read();
            }
        }
        // TODO else default initialization!
    }
}

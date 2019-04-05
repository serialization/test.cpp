//
// Created by Timm Felden on 05.04.19.
//

#include <future>

#include "../api/File.h"
#include "../streams/FileOutputStream.h"
#include "../fieldTypes/HullType.h"

#include "Pool.h"
#include "Writer.h"
#include "../fieldTypes/SingleArgumentType.h"
#include "../fieldTypes/MapType.h"

using namespace ogss::internal;
using ogss::streams::BufferedOutStream;
using ogss::fieldTypes::HullType;

Writer::Writer(api::File *state, streams::FileOutputStream &out) {
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


    /**
     * *************** * T F * ****************
     */
    int awaitBuffers;
    {
        // write T and F to a buffer, while S is written
        BufferedOutStream *const buffer = new BufferedOutStream();

        // @note here, the field data write tasks will be started already
        awaitBuffers = writeTF(state, *buffer);
        SB.get();

        // write buffered TF-blocks
        out.write(buffer);
    }

    SK_TODO + "consume buffers!";
}

static inline void attr(AbstractPool *const p, BufferedOutStream &out) {
    out.i8(0);
}

static inline void attr(FieldDeclaration *const p, BufferedOutStream &out) {
    out.i8(0);
}

uint32_t Writer::writeTF(api::File *const state, BufferedOutStream &out) {
    uint32_t awaitHulls;

    std::vector<DataField *> fieldQueue;
    StringPool *const string = (StringPool *) state->strings;

    /**
     * *************** * T Class * ****************
     */

    // calculate new bpos, sizes, object IDs and compress data arrays
    {
        std::vector<std::future<void>> barrier;

        const int classCount = state->classCount;
        int *const bpos = new int[classCount];
        int bases = 0;
        for (int i = 0; i < classCount; i++) {
            AbstractPool *const p = state->classes[i];
            if (nullptr == p->super) {
                bases++;
                barrier.push_back(std::async(std::launch::async,
                                             compress, p, bpos));
            }
        }

        // write count of the type block
        out.v64(classCount);

        // initialize local state before waiting for compress
        fieldQueue.reserve(2 * classCount);

        // await jobs
        for (auto &f : barrier)
            f.get();

        delete[] bpos;

        // write types
        for (int i = 0; i < classCount; i++) {
            AbstractPool *const p = state->classes[i];
            out.v64(string->id(p->name));
            out.v64(p->staticDataInstances);
            attr(p, out);
            if (nullptr == p->super)
                out.i8(0);
            else {
                // superID
                out.v64(p->super->typeID - 9);
                // our bpo
                out.v64(p->bpo);
            }

            out.v64((int) p->dataFields.size());

            // add field to queues for description and data tasks
            for (DataField *f : p->dataFields) {
                fieldQueue.push_back(f);
            }
        }

    }

    /**
     * *************** * T Container * ****************
     */

    // write count of the type block
    {
        const int containerCount = state->containerCount;

        // number of containers written to disk
        uint32_t count = 0;

        // set deps and calculate count
        for (int i = 0; i < containerCount; i++) {
            HullType *const c = state->containers[i];
            if (c->maxDeps != 0) {
                c->deps = c->maxDeps;
                count++;
            }
        }
        if (string->maxDeps != 0) {
            awaitHulls = 1;
            string->deps = string->maxDeps;
        }
        awaitHulls += count;

        out.v64((int) count);
        for (int i = 0; i < containerCount; i++) {
            HullType *const c = state->containers[i];
            if (c->maxDeps != 0) {
                c->writeDecl(out);
            }
        }
    }

    // note: we cannot start field jobs immediately because they could decrement deps to 0 multiple times in that
    // case
    {
        std::lock_guard<std::mutex> rLock(resultLock);
        for (DataField *f : fieldQueue) {
            results.push_back(std::async(std::launch::async,
                                         writeField, this, f));
        }
    }

    /**
     * *************** * T Enum * ****************
     */

    if (state->enumCount) {
        SK_TODO + "enums";
        //    // write count of the type block
        //    out.v64(state.enums.length);
        //    for (EnumPool< ?> p :
        //    state.enums) {
        //        out.v64(string.id(p.name));
        //        out.v64(p.values.length);
        //        for (EnumProxy< ?> v :
        //        p.values) {
        //            out.v64(string.id(v.name));
        //        }
        //    }
    } else
        out.i8(0);

    /**
     * *************** * F * ****************
     */

    for (FieldDeclaration *f : fieldQueue) {
        // write info
        out.v64(string->id(f->name));
        out.v64(f->type->typeID);
        attr(f, out);
    }

    out.close();

    // fields + hull types
    return fieldQueue.size() + awaitHulls;
}

void Writer::compress(AbstractPool *const base, int *bpos) {
    // create our part of the bpo map
    {
        int next = 0;
        AbstractPool *p = base;

        do {
            bpos[p->typeID - 10] = next;
            const int s = p->staticSize() - p->deletedCount;
            p->cachedSize = s;
            next += s;
            p = p->next;
        } while (p);
    }

    // calculate correct dynamic size for all sub pools (in reverse order)
    {
        // @note this can only happen, if there is a class
        AbstractPool *const *cs = base->owner->classes + base->owner->classCount - 1;
        AbstractPool *p;
        while (base != (p = *(cs--))) {
            if (base == p->base) {
                p->super->cachedSize += p->cachedSize;
            }
        }
    }

    // from now on, size will take deleted objects into account, thus d may
    // in fact be smaller then data!
    Object **tmp = ((Pool<Object> *) base)->data;
    base->allocateData();
    Object **d = ((Pool<Object> *) base)->data;
    ((Pool<Object> *) base)->data = tmp;
    ObjectID id = 1;

    {
        auto is = base->allObjects();
        while (is->hasNext()) {
            Object *const i = is->next();
            if (0 != i->id) {
                d[id] = i;
                i->id = id;
                id++;
            } else {
                SK_TODO + "delete";
            }
        }
    }

    // update after compress for all sub-pools
    AbstractPool *p = base;

    do {
        p->resetOnWrite(d);
        p->bpo = bpos[p->typeID - 10];
    } while ((p = p->next));
}

BufferedOutStream *Writer::writeField(Writer *self, DataField *f) {
    BufferedOutStream *buffer = new BufferedOutStream();

    bool discard = true;

    try {
        AbstractPool *owner = f->owner;
        int i = owner->bpo;
        int h = i + owner->cachedSize;

        // any empty field will be discarded
        if (i != h) {
            buffer->v64(f->fieldID);
            discard = f->write(i, h, *buffer);
        }

        if (auto ht = dynamic_cast<HullType *>((FieldType *) f->type)) {
            if (0 == --ht->deps) {
                std::lock_guard<std::mutex> rLock(self->resultLock);
                self->results.push_back(std::async(std::launch::async,
                                                   writeHull, self, ht));
            }
        }
    } catch (std::exception &e) {
        std::lock_guard<std::mutex> errLock(self->resultLock);

        self->errors.emplace_back(e.what());
    } catch (...) {
        self->errors.emplace_back("write task non-standard crash");
    }

    // return the buffer in any case to ensure that there is a
    // buffer on error
    if (discard) {
        delete buffer;
        buffer = nullptr;
    } else {
        buffer->close();
    }

    return buffer;
}


BufferedOutStream *Writer::writeHull(Writer *self, HullType *t) {
    BufferedOutStream *buffer = new BufferedOutStream();

    bool discard = true;

    try {
        buffer->v64(t->fieldID);
        discard = t->write(*buffer);

        if (auto p = dynamic_cast<fieldTypes::SingleArgumentType *>(t)) {
            if (auto bt = dynamic_cast<HullType *>(p->base)) {
                if (0 == --bt->deps) {
                    std::lock_guard<std::mutex> rLock(self->resultLock);
                    self->results.push_back(std::async(std::launch::async,
                                                       writeHull, self, bt));
                }
            }
        } else if (dynamic_cast<StringPool *>(t)) {
            // nothing to do (in fact we cant type check a MapType)
        } else {
            fieldTypes::MapType<api::Box, api::Box> *p = (fieldTypes::MapType<Box, Box> *) t;
            if (auto bt = dynamic_cast<HullType *>(p->keyType)) {
                if (0 == --bt->deps) {
                    std::lock_guard<std::mutex> rLock(self->resultLock);
                    self->results.push_back(std::async(std::launch::async,
                                                       writeHull, self, bt));
                }
            }
            if (auto bt = dynamic_cast<HullType *>(p->valueType)) {
                if (0 == --bt->deps) {
                    std::lock_guard<std::mutex> rLock(self->resultLock);
                    self->results.push_back(std::async(std::launch::async,
                                                       writeHull, self, bt));
                }
            }
        }
    } catch (std::exception &e) {
        std::lock_guard<std::mutex> errLock(self->resultLock);

        self->errors.push_back(e.what());
    } catch (...) {
        self->errors.push_back("write task non-standard crash");
    }

    // return the buffer in any case to ensure that there is a
    // buffer on error
    if (discard) {
        delete buffer;
        buffer = nullptr;
    } else {
        buffer->close();
    }

    return buffer;
}
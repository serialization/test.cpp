//
// Created by Timm Felden on 29.03.19.
//

#include "Creator.h"
#include "../fieldTypes/HullType.h"
#include "../internal/AutoField.h"

using namespace ogss::internal;

Creator::Creator(const std::string &path, const ogss::internal::PoolBuilder &pb)
        : StateInitializer(path, nullptr, pb) {
    guard.reset(new std::string());

    // Create Classes
    {
        int index = 0;
        int THH = 0;
        // the index of the next known class at index THH
        // @note to self: in C++ this should be string*[32]
        int nextID[50];
        String nextName = pb.name(0);

        AbstractPool *p = nullptr, *last = nullptr;
        while (nextName) {
            if (0 == THH) {
                last = nullptr;
                p = pb.make(nextID[0]++, index++);
            } else {
                p = p->makeSub(nextID[THH]++, index++, nullptr);
            }

            SIFA[nsID++] = p;
            classes.push_back(p);
            Strings->addLiteral(p->name);

            // set next
            if (last) {
                last->next = p;
            }
            last = p;

            // move to next pool
            {
                // try to move down to our first child
                nextName = p->nameSub(nextID[++THH] = 0);

                // move up until we find a next pool
                while (nullptr == nextName && THH != 1) {
                    p = p->super;
                    nextName = p->nameSub(nextID[--THH]);
                }
                // check at base type level
                if (nullptr == nextName) {
                    nextName = pb.name(nextID[--THH]);
                }
            }
        }
    }

    // Execute known container constructors
    ogss::TypeID tid = 10 + classes.size();
    {
        uint32_t kcc;
        for (int i = 0; -1u != (kcc = pb.kcc(i)); i++) {
            uint32_t kkind = (kcc >> 30u) & 3u;
            FieldType *kb1 = SIFA[kcc & 0x7FFFu];
            FieldType *kb2 = 3 == kkind ? SIFA[(kcc >> 15u) & 0x7FFFu] : nullptr;
            HullType *r = pb.makeContainer(kcc, tid++, kb1, kb2);
            SIFA[nsID++] = r;
            r->fieldID = nextFieldID++;
            containers.push_back(r);
        }
    }

    // Construct known enums
    {
        int ki = 0;
        //TODO EnumPool< ?> r;
        String nextName = pb.enumName(ki);
        // create remaining known enums
        while (nullptr != nextName) {
            SK_TODO;
            //            r = new EnumPool(tid++, nextName, null, pb.enumMake(ki++));
            //            Strings.add(r.name);
            //            for (EnumProxy< ?> n :
            //            r.values) {
            //                Strings.add(n.name);
            //            }
            //            enums.add(r);
            //            SIFA[nsID++] = r;
            //            nextName = pb.enumName(ki);
        }
    }

    // Create Fields
    for (AbstractPool *p : classes) {
        String f;
        for (int i = 0; (f = p->KFN(i)); i++) {
            Strings->addLiteral(f);

            FieldDeclaration *const fd = p->KFC(i, SIFA, nextFieldID);

            if (!dynamic_cast<AutoField *>(fd)) {
                nextFieldID++;

                // increase maxDeps
                if (auto h = dynamic_cast<HullType *>(const_cast<FieldType *>(fd->type))) {
                    h->maxDeps++;
                }
            }
        }
    }

    fixContainerMD();
}
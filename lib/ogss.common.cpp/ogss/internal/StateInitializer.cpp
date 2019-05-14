//
// Created by Timm Felden on 28.03.19.
//

#include "Creator.h"
#include "EnumPool.h"
#include "SeqParser.h"
#include "StateInitializer.h"

#include "../internal/StringPool.h"
#include "../fieldTypes/AnyRefType.h"
#include "../fieldTypes/BuiltinFieldType.h"
#include "../fieldTypes/SingleArgumentType.h"
#include "../fieldTypes/MapType.h"

using namespace ogss::internal;
using namespace ogss::fieldTypes;

StateInitializer *StateInitializer::make(
        const std::string &path, const PoolBuilder &pb, int mode) {
    std::unique_ptr<StateInitializer> init(nullptr);
    if (mode & api::ReadMode::create)
        init.reset(new Creator(path, pb));
    else {
        FileInputStream *fs = new FileInputStream(path);

        if (fs->size() < SEQ_PARSER_LIMIT)
            init.reset(new SeqParser(path, fs, pb));
        else
            SK_TODO + "par parser";
        //        else
        //            init = new ParParser(path, fs, pb);

        ((Parser *) init.get())->parseFile(fs);
    }
    init->canWrite = 0 == (mode & api::WriteMode::readOnly);
    return init.release();
}

StateInitializer::StateInitializer(const std::string &path, FileInputStream *in,
                                   const PoolBuilder &pb)
        : path(path), in(in), canWrite(true),
          guard(),
          classes(), containers(), enums(),
          strings(new StringPool(pb.getSK())),
          anyRef(new AnyRefType(strings, &classes)),
          SIFA(new FieldType *[pb.sifaSize]),
          sifaSize(pb.sifaSize),
          nsID(10),
          nextFieldID(1) {

    SIFA[0] = (FieldType *) &BoolType;
    SIFA[1] = (FieldType *) &I8;
    SIFA[2] = (FieldType *) &I16;
    SIFA[3] = (FieldType *) &I32;
    SIFA[4] = (FieldType *) &I64;
    SIFA[5] = (FieldType *) &V64;
    SIFA[6] = (FieldType *) &F32;
    SIFA[7] = (FieldType *) &F64;
    SIFA[8] = anyRef;
    SIFA[9] = strings;
}

void StateInitializer::fixContainerMD() {
    // increase deps caused by containsers whose maxDeps is nonzero
    for (HullType *c : containers) {
        if (c->maxDeps != 0) {
            if (auto cc = dynamic_cast<SingleArgumentType *>(c)) {
                FieldType *b = cc->base;
                if (auto h = dynamic_cast<HullType *>(b)) {
                    h->maxDeps++;
                }
            } else {
                auto m = (MapType<api::Box, api::Box> *) c;
                if (auto b = dynamic_cast<HullType *>(m->keyType)) {
                    b->maxDeps++;
                }
                if (auto b = dynamic_cast<HullType *>(m->valueType)) {
                    b->maxDeps++;
                }
            }
        }
    }
}

StateInitializer::~StateInitializer() {
    // delete all accumulated type information iff the state initializer has not been consumed
    if (strings) {
        delete strings;
        delete anyRef;

        for (auto c : classes) {
            delete c;
        }
        for (auto c : containers) {
            delete c;
        }
        for (auto c : enums) {
            delete c;
        }
    }

    delete[] SIFA;
}

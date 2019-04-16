//
// Created by Timm Felden on 04.11.15.
//

#include "../fieldTypes/AnyRefType.h"
#include "../internal/StateInitializer.h"
#include "../internal/Writer.h"
#include "../streams/FileOutputStream.h"
#include "File.h"

#include <atomic>
#include <iostream>

using namespace ogss;
using namespace api;
using namespace internal;

// file.cpp knows anything anyway, so we realize global constants here to speed-up compilation
namespace ogss {
    namespace fieldTypes {
        BoolFieldType BoolType;
        I8FieldType I8;
        I16FieldType I16;
        I32FieldType I32;
        I64FieldType I64;
        V64FieldType V64;
        F32FieldType F32;
        F64FieldType F64;
    }
}

File::File(internal::StateInitializer *init)
        : guard(*init->guard),
          strings(init->Strings),
          anyRef(init->AnyRef),
          classCount(init->classes.size()),
          classes(new AbstractPool *[classCount]),
          containerCount(init->containers.size()),
          containers(new HullType *[containerCount]),
          enumCount(0),
          enums(nullptr),
          TBN(nullptr),
          fromFile(init->in.release()),
          currentWritePath(init->path),
          canWrite(init->canWrite),
          SIFA({}) {

    // release complex builtin types
    init->Strings = nullptr;
    init->AnyRef = nullptr;

    for (size_t i = 0; i < classCount; i++) {
        auto t = init->classes[i];
        t->owner = this;
        const_cast<AbstractPool **>(classes)[i] = t;
    }
    for (size_t i = 0; i < containerCount; i++) {
        auto t = init->containers[i];
        const_cast<HullType **>(containers)[i] = t;
    }
}

File::~File() {
    for (size_t i = 0; i < classCount; i++) {
        delete classes[i];
    }
    delete[] classes;

    delete anyRef;
    delete strings;

    delete TBN;
    delete fromFile;
}

void File::check() {
    // TODO type checks!
    //    // TODO lacks type and unknown restrictions
    //
    //    // collected checked fields
    //    std::vector<FieldDeclaration *> fields;
    //    for (auto &pair : *TBN) {
    //        const auto p = pair.second;
    //        for (const auto f : p->dataFields) {
    //            if (f->hasRestrictions())
    //                fields.push_back(f);
    //        }
    //    }
    //
    //    std::atomic<bool> failed;
    //    failed = false;
    //
    //    // @note this should be more like, each pool is checking its type restriction, aggregating its field restrictions,
    //    // and if there are any, then they will all be checked using (hopefully) overridden check methods
    //#pragma omp parallel for
    //    for (size_t i = 0; i < fields.size(); i++) {
    //        const auto f = fields[i];
    //        if (!f->check()) {
    //            std::cerr << "Restriction check failed for " << *(f->owner->name) << "." << *(f->name) << std::endl;
    //            failed = true;
    //        }
    //    }
    //
    //    if (failed)
    //        throw SkillException("check failed");
}

void File::changePath(std::string path) {
    if (currentWritePath != path) {
        currentWritePath = path;
        canWrite = true;
    }
}

const std::string &File::currentPath() const {
    return currentWritePath;
}


void File::changeMode(WriteMode newMode) {
    if (newMode == WriteMode::readOnly)
        canWrite = false;
    else {
        if (!canWrite)
            throw std::invalid_argument("this file is read-only");
    }
}


void File::loadLazyData() {
    // check if the file input stream is still open
    if (!fromFile)
        return;

    // ensure that strings are loaded
    ((StringPool *) strings)->loadLazyData();

    // TODO
    //                // ensure that lazy fields have been loaded
    //                for (Pool<?> p : classes)
    //                for (FieldDeclaration<?, ?> f : p.dataFields)
    //                if (f instanceof LazyField<?, ?>)
    //                ((LazyField<?, ?>) f).ensureLoaded();
    //

    // close the file input stream and ensure that it is not read again
    delete fromFile;
    fromFile = nullptr;
}

void File::flush() {
    if (!canWrite)
        throw std::invalid_argument("this file is read-only");

    loadLazyData();

    streams::FileOutputStream out(currentPath());
    internal::Writer write(this, out);
}

void File::close() {
    flush();
    changeMode(readOnly);
}
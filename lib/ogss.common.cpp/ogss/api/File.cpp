//
// Created by Timm Felden on 04.11.15.
//

#include "File.h"
#include "../fieldTypes/AnyRefType.h"
#include "../streams/FileOutputStream.h"
#include "../internal/StateInitializer.h"

#include <atomic>
#include <iostream>

using namespace ogss;
using namespace api;
using namespace internal;

File::File(internal::StateInitializer *init)
        : guard(*init->guard),
          strings(init->Strings),
          anyRef(init->AnyRef),
          classCount(init->classes.size()),
          classes(new AbstractPool *[classCount]),
          containerCount(0),
          containers(nullptr),
          enumCount(0),
          enums(nullptr),
          TBN(nullptr),
          fromFile(init->in.release()),
          currentWritePath(fromFile->getPath()),
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
}

File::~File() {
    for (int i = 0; i < classCount; i++) {
        delete classes[i];
    }
    delete[] classes;

    delete anyRef;
    delete strings;

    if (TBN) {
        delete TBN;
    }
    if (fromFile)
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


void File::flush() {
    if (!canWrite)
        throw std::invalid_argument("this file is read-only");

    SK_TODO;
    // TODO FileWriter::write(this, currentPath());
}

void File::close() {
    flush();
    changeMode(readOnly);
}
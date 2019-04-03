//
// Created by Timm Felden on 01.04.19.
//

#ifndef OGSS_TEST_CPP_AUTOFIELD_H
#define OGSS_TEST_CPP_AUTOFIELD_H

#include "FieldDeclaration.h"

namespace ogss {
    namespace internal {
        /**
         * A field that is transient, i.e. its contents is not serialized.
         * The name is kept for historic reasons.
         *
         * @note AutoFields need a representation in OGSS RTTI to detect collisions with
         * fields of the same name in a file.
         *
         * @author Timm Felden
         */
        class AutoField : public FieldDeclaration {
            AutoField(const FieldType *const type, api::String const name,
                      const TypeID fieldID, AbstractPool *const owner)
                    : FieldDeclaration(type, name, fieldID, owner) {}

            bool check() const final {
                return true;// TODO
            }
        };
    }
}

#endif //OGSS_TEST_CPP_AUTOFIELD_H

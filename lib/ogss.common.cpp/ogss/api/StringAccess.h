//
// Created by Timm Felden on 04.11.15.
//

#ifndef SKILL_CPP_COMMON_STRINGACCESS_H
#define SKILL_CPP_COMMON_STRINGACCESS_H

#include "String.h"
#include "../fieldTypes/BuiltinFieldType.h"
#include "../streams/FileOutputStream.h"
#include "../fieldTypes/HullType.h"

namespace ogss {
    namespace api {
        class File;

        /**
         * This class is the public API for string manipulation in a SKilL file.
         *
         * @author Timm Felden
         */
        struct StringAccess : public fieldTypes::HullType {
            /**
             * Adds a string ensuring that no copy with the same image is created
             */
            virtual String add(const char *target) = 0;

            /**
             * Adds a string ensuring that no copy with the same image is created
             */
            virtual String add(const char *target, int length) = 0;

        protected:
            explicit StringAccess(TypeID tid) : HullType(tid, -1) {};

            virtual ~StringAccess() {};

            friend class File;
        };
    }
}

#endif //SKILL_CPP_COMMON_STRINGACCESS_H

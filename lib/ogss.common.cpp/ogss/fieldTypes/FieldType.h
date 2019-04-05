//
// Created by Timm Felden on 20.11.15.
//

#ifndef SKILL_CPP_COMMON_FIELDTYPE_H
#define SKILL_CPP_COMMON_FIELDTYPE_H

#include "../common.h"
#include "../api/Box.h"
#include "../internal/RTTIBase.h"
#include "../streams/BufferedOutStream.h"
#include "../streams/MappedInStream.h"
#include "../streams/MappedOutStream.h"

namespace ogss {
    namespace streams {
        class MappedOutStream;
    }

    /**
     * this namespace contains field type information used by skill
     */
    namespace fieldTypes {

        /**
         * Field types as used in reflective access.
         *  @author Timm Felden
         *  @note field types have a mental type T that represents the base type of the field
         */
        class FieldType : public internal::RTTIBase {

        protected:
            FieldType(const TypeID typeID) : typeID(typeID) {}

        public:
            virtual ~FieldType() {}

            //! the id of this type
            const TypeID typeID;

            /**
             * read a box from a stream using the correct read implementation
             */
            virtual api::Box r(streams::InStream &in) const = 0;

            /**
             * write a box to a stream
             */
            virtual bool w(api::Box v, streams::BufferedOutStream &out) const = 0;
        };
    }
}


#endif //SKILL_CPP_COMMON_FIELDTYPE_H

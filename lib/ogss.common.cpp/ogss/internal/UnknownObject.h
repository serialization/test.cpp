//
// Created by Timm Felden on 24.11.15.
//

#ifndef SKILL_CPP_COMMON_UNKNOWNOBJECT_H
#define SKILL_CPP_COMMON_UNKNOWNOBJECT_H

#include "../api/Object.h"
#include "../api/Exception.h"
#include "AbstractPool.h"

namespace ogss {
    using api::String;
    namespace internal {
        template<class T>
        class Book;

        class UnknownObject : public api::Object, public api::NamedObj {

            //! bulk allocation constructor
            UnknownObject() : api::NamedObj(nullptr) {};

            UnknownObject(ObjectID id, const AbstractPool *owner)
                    : api::NamedObj(owner) { this->id = id; }

            friend class Book<UnknownObject>;

        public:

            virtual void prettyString(std::ostream &os) const {
                os << *this->pool->name << "#" << id << "(unknown)";
            }
        };
    }
}


#endif //SKILL_CPP_COMMON_UNKNOWNOBJECT_H

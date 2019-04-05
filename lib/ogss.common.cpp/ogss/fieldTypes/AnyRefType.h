//
// Created by Timm Felden on 01.12.15.
//

#ifndef SKILL_CPP_COMMON_ANNOTATIONTYPE_H
#define SKILL_CPP_COMMON_ANNOTATIONTYPE_H

#include "../internal/AbstractPool.h"
#include "BuiltinFieldType.h"
#include "../api/File.h"
#include "../internal/StringPool.h"
#include <memory>

namespace ogss {
    using streams::InStream;
    namespace fieldTypes {

        /**
         * the implementation of annotations may not be fast (at all)
         *
         * @note maybe, the skill names returned by instances should point to
         * their pool's references, so that we can create an internal mapping from
         * const char* ⇒ type, which is basically a perfect hash map, because strings
         * will then be unique and we no longer have to care for identical copies
         */
        class AnyRefType : public BuiltinFieldType<void *, 8> {
            /**
             * @note pools in this vector are owned by annotation, as long as owner is nullptr.
             * That way, we can omit unique_pointers.
             */
            std::vector<internal::AbstractPool *> *const fdts;
            /**
             * annotations will not be queried from outside, thus we can directly use char* obtained from
             * skill string pointers
             */
            File *owner;

        public:
            AnyRefType(std::vector<internal::AbstractPool *> *fdts)
                    : fdts(fdts), owner(nullptr) {}

            virtual ~AnyRefType() {
            }

            api::Box r(streams::InStream &in) const final {
                api::Box r = {};
                TypeID t = (TypeID) in.v32();
                if (!t)
                    return r;

                ObjectID id = (ObjectID) in.v32();

                SK_TODO;
                // TODO likely not even close to correct!
                r.anyRef = fdts->at(t - 2)->getAsAnnotation(id);

                return r;
            }

            bool w(api::Box target, streams::BufferedOutStream &out) const final {
                if (target.anyRef) {
                    if (Object *ref = dynamic_cast<Object *>(target.anyRef)) {
                        out.v64(owner->pool(ref)->typeID);
                        out.v64(target.anyRef->id);
                    } else {
                        out.i8(1);
                        out.v64(owner->strings->id(target.string));
                    }
                    return false;
                } else {
                    out.i8(0);
                    return true;
                }
            }

            virtual bool requiresDestruction() const {
                return false;
            }
        };
    }
}
#endif //SKILL_CPP_COMMON_ANNOTATIONTYPE_H

//
// Created by Timm Felden on 24.11.15.
//

#ifndef SKILL_CPP_COMMON_SUBPOOL_H
#define SKILL_CPP_COMMON_SUBPOOL_H

#include "../api/Builder.h"
#include "Pool.h"

namespace ogss {
    namespace internal {

        /**
         * A generic sub pool class that creates new objects via reflection to reduce the amount of generated code.
         *
         * @author Timm Felden
         */
        template<class T>
        class SubPool final : public Pool<T> {

            void allocateInstances() final {
                SK_TODO;
                //                int i = bpo, j;
                //                final int high = i + staticDataInstances;
                //                try {
                //                    Constructor<T> make = cls.getConstructor(Pool.class, int.class);
                //                    while (i < high) {
                //                        data[i] = make.newInstance(this, j = (i + 1));
                //                        i = j;
                //                    }
                //                } catch (Exception e) {
                //                    throw new RuntimeException("internal error", e);
                //                }
            }

            AbstractPool *
            makeSub(ogss::TypeID index, String name, std::unordered_set<TypeRestriction *> *restrictions) final {
                return new SubPool<T>(index, this, name, restrictions);
            }

        public:
            SubPool(TypeID poolIndex, AbstractPool *super, String name,
                    ::std::unordered_set<::ogss::restrictions::TypeRestriction *> *restrictions)
                    : Pool<T>(poolIndex, super, name, restrictions, 0) {}


            T *make() override {
                T *rval = this->book->next();
                new(rval) T(-1, this);
                this->newObjects.push_back(rval);
                return rval;
            };

            /**
             * @return the most abstract builder to prevent users from using builders on unknown types
             */
            api::Builder *build() final {
                return new api::Builder(this->make());
            }
        };
    }
}


#endif //SKILL_CPP_COMMON_SUBPOOL_H

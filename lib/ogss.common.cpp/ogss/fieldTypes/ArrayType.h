//
// Created by Timm Felden on 01.04.19.
//

#ifndef OGSS_TEST_CPP_ARRAYTYPE_H
#define OGSS_TEST_CPP_ARRAYTYPE_H

#include "SingleArgumentType.h"
#include "../api/Arrays.h"

namespace ogss {
    namespace fieldTypes {

        /**
         * An abstract array type. Operations that require knowledge of the actual type are
         * split to the Implementation part. The type parameter is the same as for the managed
         * Array class.
         */
        template<typename T>
        class ArrayType final : public SingleArgumentType {

            BlockID allocateInstances(int count, streams::MappedInStream *in) final {

                // check for blocks
                if (count >= HD_Threshold) {
                    const BlockID block = in->v32();

                    // note: in contrast to Java, there is no easy way to do synchronized(this)
                    // Also, allocation is faster compared to Java
                    // Lastly, a container block cannot be dropped if size is nonzero
                    // Hence, we will allocate all instances in block 0

                    if (block)
                        return block;
                    // else, behave as if there were no blocks
                }
                // else, no blocks
                idMap.reserve(count);
                while (count-- != 0)
                    idMap.push_back(new api::Array<T>());
                return 0;
            }

            void read(BlockID block, streams::MappedInStream *in) final {
                size_t i = block * HD_Threshold;
                const size_t end = std::min(idMap.size(), i + HD_Threshold);
                while (++i < end) {
                    auto xs = (api::Array<T> *) idMap[i];
                    int s = in->v32();
                    xs->reserve(s);
                    while (s-- != 0) {
                        xs->push_back(api::unbox<T>(base->r(*in)));
                    }
                }
            }

            bool write(streams::BufferedOutStream *out) final {
                const int count = idMap.size() - 1;
                if (0 != count) {
                    out->v64(count);
                    for (int i = 1; i <= count; i++) {
                        auto xs = (api::Array<T> *) idMap[i];
                        out->v64((int) xs->size());
                        for (T x : *xs) {
                            base->w(api::box(x), out);
                        }
                    }
                    return false;
                }
                return true;
            }

            void writeDecl(streams::BufferedOutStream &out) const final {
                out.i8(0);
                out.v64(base->typeID);
            }

        public:
            ArrayType(TypeID tid, uint32_t kcc, FieldType *const base)
                    : SingleArgumentType(tid, kcc, base) {}

            ~ArrayType() final {
                for (void *v : idMap)
                    delete (api::Array<T> *) v;
            }

            /// simplify code generation
            inline api::Array<T> *read(streams::InStream &in) {
                return (api::Array<T> *) r(in).array;
            }
        };
    }
}


#endif //OGSS_TEST_CPP_ARRAYTYPE_H

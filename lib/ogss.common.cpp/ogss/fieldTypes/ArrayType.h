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
            streams::MappedInStream *in;

            void allocateInstances(int count, streams::MappedInStream *map) final {
                in = map;
                idMap.reserve(count);
                while (count-- != 0)
                    idMap.push_back(new api::Array<T>());
            }

            void read() final {
                const int count = idMap.size() - 1;
                for (int i = 1; i <= count; i++) {
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
                    : SingleArgumentType(tid, kcc, base), in(nullptr) {}

            ~ArrayType() final {
                delete in;
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

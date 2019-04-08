//
// Created by Timm Felden on 01.04.19.
//

#ifndef OGSS_TEST_CPP_MAPTYPE_H
#define OGSS_TEST_CPP_MAPTYPE_H


#include "../api/Maps.h"
#include "FieldType.h"

namespace ogss {
    namespace fieldTypes {
        template<typename K, typename V>
        class MapType final : public HullType {
        public:
            FieldType *const keyType;
            FieldType *const valueType;

        protected:

            /**
             * Hull-Reads cache in stream between allocate and read.
             */
            streams::MappedInStream *in;

            void allocateInstances(int count, streams::MappedInStream *map) final {
                in = map;
                idMap.reserve(count);
                while (count-- != 0)
                    idMap.push_back(new api::Map<K, V>());
            }

            void read() final {
                const int count = idMap.size() - 1;
                for (int i = 1; i <= count; i++) {
                    auto xs = (api::Map<K, V> *) idMap[i];
                    int s = in->v32();
                    xs->reserve(s);
                    while (s-- != 0) {
                        (*xs)[api::unbox<K>(keyType->r(*in))] = api::unbox<V>(valueType->r(*in));
                    }
                }
            }

            bool write(streams::BufferedOutStream *out) final {
                const int count = idMap.size() - 1;
                if (0 != count) {
                    out->v64(count);
                    for (int i = 1; i <= count; i++) {
                        auto xs = (api::Map<K, V> *) idMap[i];
                        out->v64((int) xs->size());
                        for (std::pair<K, V> x : *xs) {
                            keyType->w(api::box(x.first), out);
                            valueType->w(api::box(x.second), out);
                        }
                    }
                    return false;
                }
                return true;
            }

            void writeDecl(streams::BufferedOutStream &out) const final {
                out.i8(3);
                out.v64(keyType->typeID);
                out.v64(valueType->typeID);
            }

        public:

            MapType(TypeID tid, uint32_t kcc, FieldType *const keyType, FieldType *const valueType)
                    : HullType(tid, kcc), in(nullptr), keyType(keyType), valueType(valueType) {};

            api::Box get(ObjectID ID) const final {
                return api::box(((0 < ID) & (ID < idMap.size()))
                                ? idMap[ID]
                                : nullptr);
            }

            /// simplify code generation
            inline api::Map <K, V> *read(streams::InStream &in) {
                return (api::Map<K, V> *) r(in).map;
            }
        };
    }
}


#endif //OGSS_TEST_CPP_MAPTYPE_H

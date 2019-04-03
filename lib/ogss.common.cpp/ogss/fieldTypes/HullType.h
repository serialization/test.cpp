//
// Created by Timm Felden on 25.03.19.
//

#ifndef SKILL_CPP_COMMON_HULLTYPE_H
#define SKILL_CPP_COMMON_HULLTYPE_H

#include <vector>
#include <unordered_map>
#include "FieldType.h"
#include "../streams/BufferedOutStream.h"
#include "../streams/InStream.h"

namespace ogss {
    namespace internal {
        class StateInitializer;

        class Creator;
    }
    namespace fieldTypes {
        class AnyRefType;

        class HullType : public FieldType {
            /**
             * get an instance by its ID
             *
             * @note This is only usable for instances with IDs and for valid IDs. This function is unrelated to Collection.get
             */
        public:
            virtual api::Box get(ObjectID ID) const = 0;

            /**
             * The KCC used to construct this type. -1 for string and for types with boxed bases.
             */
            const uint32_t kcc;

        private:
            /**
             * The field ID used by this hull on write.
             */
            int fieldID{};

            /**
             * The number of other fields currently depending on this type. It is set by Writer on serialization in Tco.
             *
             * @note If another field reduces deps to 0 it has to start a write job for this type.
             * @note This is in essence reference counting on an acyclic graph while writing data to disk.
             */
            int deps = 0;

            /**
             * The maximal, i.e. static, number of serialized fields depending on this type.
             *
             * @note Can be 0.
             * @note If 0, the HullType is excluded from serialization.
             */
            int maxDeps = 0;

        protected:
            /**
             * get object by ID
             *
             * @note should be Array[T]
             */
            mutable std::vector<void *> idMap;

            /**
             * get ID from Object
             *
             * @note should be HashMap[T, ObjectID]
             */
            mutable std::unordered_map<const void *, ObjectID> IDs;

            /**
             * forget all IDs
             */
            void resetIDs() {
                IDs.clear();

                // throw away id map, as it is no longer valid
                idMap.clear();
                idMap.push_back(nullptr);
            }


            /**
             * Read the hull data from the stream. Abstract, because the inner loop is type-dependent anyway.
             *
             * @note the fieldID is written by the caller
             * @return true iff hull shall be discarded (i.e. it is empty)
             */
            virtual void read() = 0;

            /**
             * Write the hull into the stream. Abstract, because the inner loop is type-dependent anyway.
             *
             * @note the fieldID is written by the caller
             * @return true iff hull shall be discarded (i.e. it is empty)
             */
            virtual bool write(streams::BufferedOutStream *out) = 0;


            explicit HullType(TypeID tid, uint32_t kcc) : FieldType(tid), kcc(kcc), idMap(), IDs() {
                idMap.push_back(nullptr);
            }

            /**
             * Return the id of the argument ref. This method is thread-safe. The id returned by this function does not change
             * per invocation.
             *
             * @note ref should be T
             */
            ObjectID id(const void *ref) const {
                if (!ref)
                    return 0;

                ObjectID r;
#pragma omp critical
                {
                    ObjectID &rval = IDs[ref];
                    if (!rval) {
                        rval = idMap.size();
                        idMap.push_back((void *) ref);
                    }
                    r = rval;
                }
                return r;
            }


            virtual api::Box r(streams::InStream &in) const override final {
                return get(in.v32());
            }

            virtual bool w(api::Box v, streams::BufferedOutStream *out) const override final {
                if (!v.anyRef) {
                    out->i8(0);
                    return true;
                }

                out->v64(id(v.anyRef));
                return false;
            }

            virtual void allocateInstances(int count, streams::MappedInStream *map) = 0;

            friend class AnyRefType;

            friend class internal::Creator;

            friend class internal::StateInitializer;
        };
    }
}


#endif //SKILL_CPP_COMMON_HULLTYPE_H

//
// Created by Timm Felden on 15.04.19.
//

#ifndef OGSS_COMMON_INTERNAL_ENUMPOOL_H
#define OGSS_COMMON_INTERNAL_ENUMPOOL_H

#include "../api/Enum.h"
#include "../fieldTypes/FieldType.h"

namespace ogss {
    namespace internal {
        class AbstractEnumPool : public FieldType {
        public:
            /**
             * Last value used for writing for loops over known values.
             * [[for(i = 0; i < last; i++)]]
             */
            const EnumBase last;

            const String name;

            /**
             * turn an enum constant into a proxy
             * @return nullptr, if the value is not legal (including UNKNOWN)
             */
            virtual AbstractEnumProxy *proxy(EnumBase target) const = 0;

            /**
             * @return begin of all instances
             */
            virtual AbstractEnumProxy *begin() = 0;

            /**
             * @return end of all instances
             */
            virtual AbstractEnumProxy *end() = 0;

        protected:
            AbstractEnumPool(int tid, String name, EnumBase last)
                    : FieldType(tid), last(last), name(name) {};
        };

        template<typename T>
        class EnumPool : public AbstractPool {
            /**
             * values as seen from the combined specification
             */
            std::vector<api::EnumProxy<T> *> values;

            /**
             * values from the perspective of the files specification, i.e. this table is used to decode values from disc
             */
            std::vector<api::EnumProxy<T> *> fileValues;

            /**
             * values from the perspective of the tools specification, i.e. this table is used to convert enum values to proxies
             */
            std::vector<api::EnumProxy<T> *> staticValues;

        public:
            /**
             * @note Shall only be called by generated PBs!
             */
            EnumPool(int tid, String name, const std::vector<String> &foundValues,
                     const String *known, const T last)
                    : AbstractPool(tid, name, last), values(), fileValues(), staticValues() {

                if (foundValues.empty()) {
                    // only known values, none from file
                    // @note we set file values anyway to get sane default values
                    staticValues.reserve(last);
                    values.reserve(last);
                    for (T i = 0; i < last; i++) {
                        values[i] = staticValues[i] = new EnumProxy<T>(i, this, known + i, i);
                    }
                } else {
                    this->fileValues.reserve(foundValues.size());

                    // check if there is a known enum associated with this pool
                    if (!known) {
                        values.reserve(foundValues.size());
                        for (int i = 0; i < values.length; i++) {
                            fileValues[i] = values[i] = new EnumProxy<T>(-1, this, foundValues[i], i);
                        }
                    } else {
                        staticValues.reserve(last);
                        values.reserve(last);

                        // merge file values and statically known values
                        int id = 0, vi = 0;
                        T ki = 0;
                        EnumProxy <T> p;
                        while ((vi < values.length) | (ki < last)) {
                            int cmp = ki < last
                                      ? (vi < values.length
                                         ? api::ogssLess::javaCMP(values[vi], known[ki].toString())
                                         : 1)
                                      : -1;

                            if (0 == cmp) {
                                p = new EnumProxy<T>(known[ki], this, values[vi], id++);
                                fileValues[vi++] = p;
                                staticValues[ki++] = p;
                                values.push_back(p);

                            } else if (cmp < 0) {
                                p = new EnumProxy<T>(-1, this, values[vi], id++);
                                fileValues[vi++] = p;
                                values.push_back(p);
                            } else {
                                p = new EnumProxy<T>(known[ki], this, known[ki].toString(), id++);
                                staticValues[ki++] = p;
                                values.push_back(p);
                            }
                        }
                    }
                }
            }
        };
    }
}


#endif //OGSS_COMMON_INTERNAL_ENUMPOOL_H

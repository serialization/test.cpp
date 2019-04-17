//
// Created by Timm Felden on 15.04.19.
//

#ifndef OGSS_COMMON_INTERNAL_ENUMPOOL_H
#define OGSS_COMMON_INTERNAL_ENUMPOOL_H

#include "../api/Enum.h"
#include "../fieldTypes/FieldType.h"

namespace ogss {
    namespace internal {
        class AbstractEnumPool : public fieldTypes::FieldType {
        public:
            /**
             * Last value used for writing for loops over known values.
             * [[for(i = 0; i < last; i++)]]
             */
            const EnumBase last;

            const api::String name;

            /**
             * turn an enum constant into a proxy
             * @return nullptr, if the value is not legal (including UNKNOWN)
             */
            virtual api::AbstractEnumProxy *proxy(EnumBase target) const = 0;

            /**
             * @return begin of all instances
             */
            virtual api::AbstractEnumProxy **begin() = 0;

            /**
             * @return end of all instances
             */
            virtual api::AbstractEnumProxy **end() = 0;

        protected:
            AbstractEnumPool(int tid, const api::String name, const EnumBase last)
                    : FieldType(tid), last(last), name(name) {};

            virtual ~AbstractEnumPool() = default;
        };

        struct Creator;
        struct Writer;

        template<typename T>
        class EnumPool : public AbstractEnumPool {
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
            EnumPool(int tid, api::String name, const std::vector<api::String> &foundValues,
                     api::String *const known, const EnumBase last)
                    : AbstractEnumPool(tid, name, last), values(), fileValues(), staticValues() {

                api::EnumProxy<T> *p;
                if (foundValues.empty()) {
                    // only known values, none from file
                    // @note we set file values anyway to get sane default values
                    staticValues.reserve(last);
                    values.reserve(last);
                    for (EnumBase i = 0; i < last; i++) {
                        p = new api::EnumProxy<T>((T) i, this, known[i], i);
                        values.push_back(p);
                        staticValues.push_back(p);
                    }
                } else {
                    this->fileValues.reserve(foundValues.size());

                    // check if there is a known enum associated with this pool
                    if (!known) {
                        values.reserve(foundValues.size());
                        for (size_t i = 0; i < values.size(); i++) {
                            p = new api::EnumProxy<T>(T::UNKNOWN, this, foundValues[i], i);
                            fileValues.push_back(p);
                            values.push_back(p);
                        }
                    } else {
                        staticValues.reserve(last);
                        values.reserve(last);

                        // merge file values and statically known values
                        int id = 0, fi = 0;
                        EnumBase ki = 0;
                        while ((fi < foundValues.size()) | (ki < last)) {
                            int cmp = ki < last
                                      ? (fi < foundValues.size()
                                         ? api::ogssLess::javaCMP(foundValues[fi], known[ki])
                                         : 1)
                                      : -1;

                            if (0 == cmp) {
                                p = new api::EnumProxy<T>((T) ki, this, foundValues[fi], id++);

                                fileValues.push_back(p);
                                fi++;
                                staticValues.push_back(p);
                                ki++;
                                values.push_back(p);

                            } else if (cmp < 0) {
                                p = new api::EnumProxy<T>(T::UNKNOWN, this, foundValues[fi], id++);
                                fileValues.push_back(p);
                                fi++;
                                values.push_back(p);
                            } else {
                                p = new api::EnumProxy<T>((T) ki, this, known[ki], id++);
                                staticValues.push_back(p);
                                ki++;
                                values.push_back(p);
                            }
                        }
                    }
                }
            }

            ~EnumPool() final {
                // all proxies from either source are in values, so delete only them
                for (api::EnumProxy<T> *v : values)
                    delete v;
            }

            api::AbstractEnumProxy *proxy(EnumBase target) const final {
                return staticValues.at(target);
            }

            api::Box r(streams::InStream &in) const final {
                api::Box r;
                r.enumProxy = fileValues.at(in.v32());
                return r;
            }

            /**
             * This is a bit of a hack, because we allow v to be both, an enum value and an EnumProxy*.
             */
            bool w(api::Box v, streams::BufferedOutStream *out) const final {
                if ((0 <= v.i64) & (v.i64 < staticValues.size())) {
                    out->v64(v.i64);
                } else {
                    // @note v can be null, iff there are no static values
                    if (v.enumProxy)
                        out->v64(v.enumProxy->id);
                    else
                        out->i8(0);
                }
            }

            api::AbstractEnumProxy **begin() final {
                return (api::AbstractEnumProxy **) values.data();
            }

            api::AbstractEnumProxy **end() final {
                return (api::AbstractEnumProxy **) (values.data() + values.size());
            }


            friend struct Creator;
            friend struct Writer;
        };
    }
}


#endif //OGSS_COMMON_INTERNAL_ENUMPOOL_H

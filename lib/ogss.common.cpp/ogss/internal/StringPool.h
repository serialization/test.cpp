//
// Created by Timm Felden on 04.11.15.
//

#ifndef SKILL_CPP_COMMON_STRINGPOOL_H
#define SKILL_CPP_COMMON_STRINGPOOL_H

#include <set>
#include <vector>
#include <unordered_set>

#include "../api/StringAccess.h"
#include "../streams/FileInputStream.h"
#include "AbstractStringKeeper.h"

namespace ogss {

    using namespace api;
    namespace internal {
        class StateInitializer;

        class Parser;

        class Writer;

        /**
         * Implementation of all string handling.
         *
         * @author Timm Felden
         */
        class StringPool : public StringAccess {

            streams::FileInputStream *in;

            /**
             * the set of known strings, including new strings
             *
             * @note having all strings inside of the set instead of just the new ones, we can optimize away some duplicates,
             * while not having any additional cost, because the duplicates would have to be checked upon serialization anyway.
             * Furthermore, we can unify type and field names, thus we do not have to have duplicate names laying around,
             * improving the performance of hash containers and name checks:)
             */
            mutable std::unordered_set<String, equalityHash, equalityEquals> knownStrings;

            /**
             * Strings known at compile time. Literals are not deleted and cannot be removed from a pool.
             */
            const AbstractStringKeeper *const literals;

            /**
             * get string by ID
             */
            mutable std::vector<String> idMap;

            /**
             * ID ⇀ (absolute offset|32, length|32) will be used if idMap contains a nullptr
             *
             * @note there is a fake entry at ID 0
             * @note offset|32 is sane as long as JVM-targets are unable to read files larger than 2GB
             */
            uint64_t *positions;

            /**
             * next legal ID, used to check access
             */
            ObjectID lastID;

            StringPool(streams::FileInputStream *in, const AbstractStringKeeper *sk);

            virtual ~StringPool();

        public:

            /**
             * add a string to the pool
             */
            virtual String add(const char *target);

            /**
             * add a string of given length to the pool
             *
             * @note this string may contain null characters
             */
            virtual String add(const char *target, int length);

            api::Box get(ObjectID ID) const {
                api::Box r;
                r.string = byID(ID);
                return r;
            }

            /**
             * search a string by id it had inside of the read file, may block if the string has not yet been read
             */
            String byID(ObjectID index) const {
                if (index <= 0) return nullptr;
                else if (index > lastID) throw std::out_of_range("index of StringPool::get too large");
                else {
                    String result = idMap[index];
                    if (nullptr == result) {
                        std::lock_guard<std::mutex> readLock(mapLock);

                        // read result
                        uint64_t off = positions[index];
                        result = in->string(off >> 32LU, (uint32_t) off, index);

                        // unify result with known strings
                        auto it = knownStrings.find(result);
                        if (it == knownStrings.end()) {
                            // a new string
                            knownStrings.insert(result);
                        } else {
                            // a string that exists already;
                            // the string cannot be from the file, so set the id
                            delete result;
                            result = *it;
                            IDs[result] = index;
                        }

                        idMap[index] = result;
                    }
                    return result;
                }
            }

            /**
             * Ensure that all Strings have been read.
             */
            void loadLazyData();

        private:

            /**
             * Read a string block
             *
             * @return the position behind the string block
             */
            size_t S(int count, ogss::streams::InStream *in);

            void read() final;

            /// id offset of the actual hull (this type also has two areas where instances are stored)
            int hullOffset;

            static void writeLiterals(StringPool *const sp, ogss::streams::FileOutputStream *out);

            bool write(ogss::streams::BufferedOutStream *) final;

            void allocateInstances(int, ogss::streams::MappedInStream *) final;

            friend class Parser;

            friend class StateInitializer;

            friend class Writer;
        };
    }
}

#endif //SKILL_CPP_COMMON_STRINGPOOL_H

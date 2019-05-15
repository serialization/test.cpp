//
// Created by Timm Felden on 04.11.15.
//

#ifndef SKILL_CPP_COMMON_STRINGPOOL_H
#define SKILL_CPP_COMMON_STRINGPOOL_H

#include <set>
#include <vector>
#include <unordered_set>

#include "AbstractStringKeeper.h"
#include "../fieldTypes/HullType.h"
#include "../streams/FileInputStream.h"

namespace ogss {

    namespace api {
        class File;
    }

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
        class StringPool final : public fieldTypes::HullType {

            ogss::streams::MappedInStream *in;

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
             * Strings used as names of types, fields or enum constants.
             *
             * @note literal strings are respective to the merged type system
             *
             * @note literal strings can alias literals->strings, in which case it must not be freed
             */
            const String *literalStrings;
            size_t literalStringCount;

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

            explicit StringPool(const AbstractStringKeeper *sk);

            ~StringPool() override;

        public:

            /**
             * convert a const char* to a string reusing already existing strings with the same image
             * @note this is essentially Javas String.intern()
             */
            api::String add(const char *img) const {
                auto r = new std::string(img);
                auto other = knownStrings.find(r);
                if (other != knownStrings.end()) {
                    delete r;
                    return *other;
                }
                // else, it is not known yet
                knownStrings.insert(r);
                return r;
            }

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
                    String result = static_cast<String>(idMap[index]);
                    if (nullptr == result) {
                        std::lock_guard<std::mutex> readLock(mapLock);

                        // read result
                        uint64_t off = positions[index];
                        result = in->string(off >> 32LU, (uint32_t) off);

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

                        idMap[index] = (void *) result;
                    }
                    return result;
                }
            }

            /**
             * Ensure that all Strings have been read.
             */
            void loadLazyData();

        private:

            void readSL(ogss::streams::FileInputStream *in);

            void read(BlockID block, streams::MappedInStream *in) final;

            /// id offset of the actual hull (this type also has two areas where instances are stored)
            int hullOffset;

            static void writeLiterals(StringPool *const sp, ogss::streams::FileOutputStream *out);

            bool write(ogss::streams::BufferedOutStream *) final;

            BlockID allocateInstances(int, ogss::streams::MappedInStream *) final;

            friend class api::File;

            friend class Parser;

            friend class StateInitializer;

            friend class Writer;
        };
    }
}

#endif //SKILL_CPP_COMMON_STRINGPOOL_H

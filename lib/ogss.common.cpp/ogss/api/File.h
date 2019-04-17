//
// Created by Timm Felden on 04.11.15.
//

#ifndef SKILL_CPP_COMMON_SKILLFILE_H
#define SKILL_CPP_COMMON_SKILLFILE_H

#include <unordered_map>
#include <memory>
#include "String.h"
#include "../fieldTypes/AbstractEnumType.h"
#include "../fieldTypes/HullType.h"
#include "../internal/AbstractPool.h"
#include "../streams/FileInputStream.h"
#include "StringAccess.h"

namespace ogss {
    namespace internal {
        class Writer;

        struct StateInitializer;
    }

    namespace api {

        /**
         * use first bit of mode
         */
        enum ReadMode : int {
            read = 0, create = 1
        };

        /**
         * use second bit of mode
         */
        enum WriteMode : int {
            write = 0, readOnly = 2
        };

        /**
         * the type of the type by name mapping
         */
        typedef std::unordered_map<
                api::String,
                internal::AbstractPool *,
                equalityHash,
                equalityEquals> typeByName_t;


        /**
         * spec independent public API for skill files
         */
        class File {

        public:
            /**
             * The guard word written in this file.
             */
            std::string guard;

            api::StringAccess *const strings;
        protected:
            fieldTypes::AnyRefType *const anyRef;
        public :
            const fieldTypes::AnyRefType *const getAnyRefType() const {
                return anyRef;
            }

        protected:


            const size_t classCount;
            /**
             * types managed by this file.
             *
             * heap allocated array of pools
             *
             * @note owns the pools
             */
            internal::AbstractPool *const *const classes;
            const size_t containerCount;
            fieldTypes::HullType *const *const containers;
            const size_t enumCount;
            internal::AbstractEnumPool *const *const enums;

            /**
             * typename -> type mapping
             */
            const typeByName_t *TBN;

            /**
             * the file we read from
             * @note null, iff no file was read, or it has been closed already
             * @note owned by this
             */
            streams::FileInputStream *fromFile;

            /**
             * our current path to write to
             */
            std::string currentWritePath;

            /**
             * True iff the state can perform write operations.
             */
            bool canWrite;

            File(internal::StateInitializer *init);

        public:
            /**
             * Will release resources of this file, but will *NOT* write changes to disk!
             */
            virtual ~File();

            /**
             * Set a new path for the file to influence future flush/close operations.
             */
            void changePath(std::string path);

            /**
             * @return the current path pointing to the file
             */
            const std::string &currentPath() const;

            /**
             * Set a new mode.
             *
             * @note not fully implemented
             */
            void changeMode(WriteMode newMode);


            /**
             * @return pool for a given type name
             */
            internal::AbstractPool *pool(api::String name) {
                if (!TBN) {
                    auto r = new typeByName_t();

                    for (auto p : *this) {
                        r->at(p->name) = p;
                    }
                    TBN = r;
                }
                return (internal::AbstractPool *) TBN->at(name);
            }

            /**
             * @return the pool corresponding to the dynamic type of the argument Obj
             */
            const internal::AbstractPool *pool(Object *ref) const {
                if (!ref) {
                    return nullptr;
                } else if (NamedObj *no = dynamic_cast<NamedObj *>(ref))
                    return no->pool;
                else
                    return (internal::AbstractPool *) this->SIFA[ref->stid() - 10];
            }

            /**
             * @return the type name of the type of an object.
             */
            inline String typeName(Object *ref) {
                return pool(ref)->name;
            }


            /**
             * @return true, iff the argument object is managed by this state
             * @note will return true, if argument is null
             * @note this operation is O(1) but kind of expensive
             */
            // TODO implementation
            bool contains(Object *ref) const;

            /**
             * ensure that the argument instance will be deleted on next flush
             *
             * @note safe behaviour for null and duplicate delete
             */
            void free(Object *ref) const {
                if (!ref || ref->isDeleted())
                    return;

                const_cast<internal::AbstractPool *>(pool(ref))->free(ref);
            }

            /**
             * Checks consistency of the current state of the file.
             *
             * @note if check is invoked manually, it is possible to fix the inconsistency and re-check without breaking the
             *       on-disk representation
             * @throws SkillException
             *             if an inconsistency is found
             */
            void check();


            /**
             * Force all lazy string and field data to be loaded from disk.
             */
            void loadLazyData();

            /**
             * Write changes to disk.
             *
             * @note this will not sync the file to disk, but it will block until all in-memory changes are written to buffers.
             * @note if check fails, then the state is guaranteed to be unmodified compared to the state before flush
             * @throws SkillException
             *             if check fails
             */
            void flush();

            /**
             * Same as flush, changeMode(readOnly).
             */
            void close();

            /**
             * start iteration over types in the state
             */
            internal::AbstractPool *const *begin() const {
                return classes;
            }

            /**
             * end iteration over types in the state
             */
            internal::AbstractPool *const *end() const {
                return classes + classCount;
            }

            /**
             * number of types in the file
             */
            size_t size() const {
                return classCount;
            }

            /**
             * Static Initialization of Fields Array.
             * The generated Files contain one field per statically known type.
             * These fields are placed right after this field.
             *
             * @note OGSS/C++ generates static_asserts that ensure the correct layout has been chosen
             * @note to self: this might be a ** pointing to the first declared field, if the C++-implementation is utter garbage :-]
             */
            fieldTypes::FieldType *const SIFA[0];

            friend class ogss::internal::Writer;
        };
    }
}

#endif //SKILL_CPP_COMMON_SKILLFILE_H

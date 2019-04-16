//
// Created by Timm Felden on 04.11.15.
//

#ifndef SKILL_CPP_COMMON_STORAGEPOOL_H
#define SKILL_CPP_COMMON_STORAGEPOOL_H

#include "AbstractPool.h"
#include "Book.h"
#include "../restrictions/TypeRestriction.h"
#include "../iterators/TypeOrderIterator.h"
#include "../iterators/DynamicDataIterator.h"
#include "../iterators/AllObjectIterator.h"

namespace ogss {
    namespace iterators {
        template<class T>
        class StaticDataIterator;

        template<class T>
        class DynamicDataIterator;
    }

    using restrictions::TypeRestriction;
    namespace internal {
        template<class T>
        class SubPool;

        class Writer;

        /**
         * @author Timm Felden
         *
         * @tparam T the type of the represented class, i.e. without a *
         */
        template<class T>
        class Pool : public AbstractPool {

        protected:
            friend class AbstractPool;

            /**
             * allocated when all instances are allocated, because by then, we can know
             * how many instances are to be read from file, which is quite helpful
             */
            Book<T> *book;

        public:
            /**
             * @note internal use only!
             *
             * @note the truth would be B*[], but this is not important now
             * @note the pointer is shifted by 1, so that access by id will get the right
             * result
             */
            T **data;

        protected:
            void allocateData() final {
                if (super) {
                    this->data = (T **) ((Pool<T> *) this->base)->data;
                } else {
                    this->data = new T *[this->cachedSize] - 1;
                }
            }

            void resetOnWrite(Object **d) final {
                // update data
                data = (T **) d;

                // update structural knowledge of data
                staticDataInstances += newObjects.size() - deletedCount;
                deletedCount = 0;
                newObjects.clear();
            }

            void allocateInstances() override {
                book = new Book<T>(staticDataInstances);
                T *page = book->firstPage();
                ObjectID i = bpo + 1;
                const auto last = i + staticDataInstances;
                for (; i < last; i++) {
                    // note: the first page consist of fresh instances. So, placement new is not required
                    page->id = i;
                    data[i] = page++;
                }
            }

            /**
             * All stored objects, which have exactly the type T. Objects are stored as arrays of field entries. The types of the
             *  respective fields can be retrieved using the fieldTypes map.
             */
            std::vector<T *> newObjects;

            //! static data iterator can traverse over new objects
            friend class iterators::StaticDataIterator<T>;

            //! dynamic data iterator can traverse over new objects
            friend class iterators::DynamicDataIterator<T>;

            virtual ObjectID newObjectsSize() const {
                return (ObjectID) newObjects.size();
            }

            Pool(TypeID typeID, AbstractPool *superPool,
                 api::String name, std::unordered_set<TypeRestriction *> *restrictions, int autoFields)
                    : AbstractPool(typeID, superPool, name, restrictions, autoFields),
                      book(nullptr),
                      data(nullptr) {}

            virtual ~Pool() {
                if (book)
                    delete book;
                if (!super)
                    delete[] (data + 1);
            }

        public:

            inline T *get(ObjectID id) const {
                // TODO check upper bound
                return id <= 0 ? nullptr : data[id];
            }

            T *make() override {
                if (!book)
                    book = new Book<T>();

                T *rval = book->next();
                new(rval) T();
                rval->id = -1;
                this->newObjects.push_back(rval);
                return rval;
            };

            std::unique_ptr<iterators::AllObjectIterator> allObjects() const final {
                return std::unique_ptr<iterators::AllObjectIterator>(
                        new iterators::AllObjectIterator::Implementation<T>(allInTypeOrder()));
            }

            iterators::StaticDataIterator<T> staticInstances() const {
                return iterators::StaticDataIterator<T>(this);
            };

            iterators::DynamicDataIterator<T> all() const {
                return iterators::DynamicDataIterator<T>(this);
            };

            iterators::TypeOrderIterator<T> allInTypeOrder() const {
                return iterators::TypeOrderIterator<T>(this);
            };

            iterators::DynamicDataIterator<T> begin() const {
                return iterators::DynamicDataIterator<T>(this);
            };

            const iterators::DynamicDataIterator<T> end() const {
                return iterators::DynamicDataIterator<T>();
            }

            friend class Writer;
        };
    }
}


#endif //SKILL_CPP_COMMON_STORAGEPOOL_H

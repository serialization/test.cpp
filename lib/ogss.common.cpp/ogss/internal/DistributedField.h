//
// Created by Timm Felden on 28.01.16.
//

#ifndef SKILL_CPP_COMMON_DISTRIBUTEDFIELD_H
#define SKILL_CPP_COMMON_DISTRIBUTEDFIELD_H

#include "DataField.h"
#include <unordered_map>

namespace ogss {
    namespace internal {

        class DistributedField : public DataField {
        protected:
            // data held as in storage pools
            // TODO shift by owner.bpo; store lastID?
            mutable api::Box *data;
            mutable std::unordered_map<const api::Object *, api::Box> newData;

        public:
            DistributedField(const FieldType *const type, api::String name,
                             const TypeID index, AbstractPool *const owner)
                    : DataField(type, name, index, owner), data(), newData() {}

            virtual ~DistributedField();

            api::Box getR(const api::Object *i) override;

            void setR(api::Object *i, api::Box v) override;

            void read(int i, int last, streams::MappedInStream &in) const override;

            bool write(int i, int last, streams::BufferedOutStream &out) const override;

            bool check() const override;
        };
    }
}


#endif //SKILL_CPP_COMMON_DISTRIBUTEDFIELD_H

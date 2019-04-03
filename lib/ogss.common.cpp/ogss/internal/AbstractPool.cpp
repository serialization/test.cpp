//
// Created by Timm Felden on 04.11.15.
//

#include "AbstractPool.h"
#include "UnknownObject.h"
#include "Pool.h"
#include "../iterators/TypeHierarchyIterator.h"

using namespace ogss;
using namespace internal;
using restrictions::TypeRestriction;

static AutoField **const noAutoFields = new AutoField *[0];

ogss::internal::AbstractPool::AbstractPool(
        TypeID typeID, AbstractPool *superPool,
        api::String const name,
        std::unordered_set<TypeRestriction *> *restrictions,
        int afCount
)
        : FieldType(typeID), restrictions(restrictions),
          name(name),
          super(superPool),
          base(superPool ? superPool->base : this),
          THH(superPool ? superPool->THH + 1 : 0),
          dataFields(),
          afCount(afCount),
          autoFields(afCount ? new AutoField *[afCount] : noAutoFields) {
}

internal::AbstractPool::~AbstractPool() {
    if (restrictions)
        delete restrictions;

    for (auto f : dataFields)
        delete f;

    if (afCount) {
        for (int i = 0; i < afCount; i++)
            delete autoFields[i];
        delete[] autoFields;
    }
}

api::Object *AbstractPool::getAsAnnotation(ObjectID id) const {
    return (((0 < id) & (id < lastID))
            ? nullptr
            : (((Pool<Object> *) this)->data[id]));
}

api::Box AbstractPool::r(streams::InStream &in) const {
    api::Box r = {};
    const auto id = (ObjectID) (in.has(9) ? in.v64checked() : in.v64());
    r.anyRef = ((0 < id & id < lastID) ? nullptr
                                       : (((Pool<Object> *) this)->data[id]));
    return r;
}

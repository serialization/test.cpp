//
// Created by Timm Felden on 01.04.19.
//

#ifndef OGSS_TEST_CPP_MAPTYPE_H
#define OGSS_TEST_CPP_MAPTYPE_H


#include "FieldType.h"

namespace ogss {
    namespace fieldTypes {
        class MapType {

        public:
            FieldType *const keyType;
            FieldType *const valueType;

        };
    }
}


#endif //OGSS_TEST_CPP_MAPTYPE_H

//
// Created by Timm Felden on 01.04.19.
//

#ifndef OGSS_TEST_CPP_ARRAYTYPE_H
#define OGSS_TEST_CPP_ARRAYTYPE_H

#include "SingleArgumentType.h"

namespace ogss {
    namespace fieldTypes {

        /**
         * An abstract array type. Operations that require knowledge of the actual type are
         * split to the Implementation part. The type parameter is the same as for the managed
         * Array class.
         */
        class ArrayType : public SingleArgumentType {


        };

        template<typename T>
        class ArrayTypeImplementation : ArrayType {

        };

    }
}


#endif //OGSS_TEST_CPP_ARRAYTYPE_H

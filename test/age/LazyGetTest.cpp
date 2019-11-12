#include <gtest/gtest.h>
#include "../common/utils.h"
#include "../../src/age/File.h"
#include "../../src/empty/File.h"

#include "../../lib/ogss.common.cpp/ogss/iterators/StaticFieldIterator.h"


using namespace common;

/*
 *
 */
TEST(LazyGetTest, test0) {
    try {
        auto sf = common::tempFile<age::api::File>();
        auto a = sf->Age->make();
        sf->close();

        std::unique_ptr<empty::api::File> sf2(empty::api::File::open(sf->currentPath()));
        auto pool = sf2->pool(sf2->strings->add(u8"Age"));
        ASSERT_EQ(1, pool->size());
        auto obj = pool->getAsAnnotation(a->ID());
        auto f = pool->fields().next();

        f->setR(obj, ::ogss::api::box(42));

    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "an exception was thrown:" << std::endl << e.what();
    }
    GTEST_SUCCEED();
}

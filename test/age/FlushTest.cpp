#include <gtest/gtest.h>
#include "../common/utils.h"
#include "../../src/age/File.h"
#include "../../src/empty/File.h"

#include "../../lib/ogss.common.cpp/ogss/iterators/StaticFieldIterator.h"

using namespace common;

/*
 * test wether instances made after flush are indeed there
 */
TEST(FlushTest, simpleflush) {
    try {
        auto sf = common::tempFile<age::api::File>();
        sf->Age->make();
        sf->flush();
        sf->Age->make();
        sf->close();

        std::unique_ptr<age::api::File> sf2(age::api::File::open(sf->currentPath()));
        ASSERT_EQ(2, sf2->Age->size());
        sf2->flush();
    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "an exception was thrown:" << std::endl << e.what();
    }
    GTEST_SUCCEED();
}

/*
 * test wether instances made after flush are indeed there, but with fields
 */
TEST(FlushTest, flushWithFields) {
    try {
        auto sf = common::tempFile<age::api::File>();

        // make a, set fields
        auto a = sf->Age->make();
        a->setAge(42);
        sf->flush();

        auto a1 = sf->Age->make();
        a1->setAge(17);
        sf->flush();

        auto sf2 = age::api::File::open(sf->currentPath());
        ASSERT_EQ(2, sf2->Age->size());
        ASSERT_EQ(42, sf2->Age->get(a->ID())->getAge());
        ASSERT_EQ(17, sf2->Age->get(a1->ID())->getAge());
        sf2->close();
    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "an exception was thrown:" << std::endl << e.what();
    }
    GTEST_SUCCEED();
}


/*
 * test whether changes applied to an instance of unknown type after flush are indeed there
 */
TEST(FlushTest, flushWithUnknown) {
    try {
        auto sf = common::tempFile<age::api::File>();
        // make a, set fields
        auto a = sf->Age->make();
        a->setAge(42);
        sf->close();

        //auto sf2 = empty::api::File::open(sf->currentPath(), ::ogss::api::WriteMode::write);
        std::unique_ptr<empty::api::File> sf2(empty::api::File::open(sf->currentPath()));
        ASSERT_EQ(1, (*sf2->begin())->size());
        sf2->flush();
        auto f = (*sf2->begin())->fields().next();
        auto o = (*sf2->begin())->getAsAnnotation(a->ID());
        f->setR(o, ::ogss::api::box(17L));
        sf2->flush();

        auto sf3 = age::api::File::open(sf->currentPath(), ::ogss::api::WriteMode::write);
        ASSERT_EQ(1, sf3->Age->size());
        ASSERT_EQ(17, sf3->Age->get(a->ID())->getAge());
        sf3->close();


    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "an exception was thrown:" << std::endl << e.what();
    }
    GTEST_SUCCEED();
}

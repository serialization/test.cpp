#include <gtest/gtest.h>
#include "../common/utils.h"
#include "../../src/aWithFields/File.h"
#include "../../src/a/File.h"

using namespace common;

/*
 * write a file, read it such that the type a is known but it's fields are unknown / lazy.
 * do nothing, write it and read it again.
 */
TEST(UnknownWriteTestA, nothing_nothing) {
    try {
        auto sf = common::tempFile<aWithFields::api::File>();
        sf->close();

        std::unique_ptr<a::api::File> sf2(a::api::File::open(sf->currentPath()));
        ASSERT_EQ(0, sf2->A->size());
        sf2->close();

        std::unique_ptr<aWithFields::api::File> sf3(aWithFields::api::File::open(sf->currentPath()));
        ASSERT_EQ(0, sf3->A->size());

    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "an exception was thrown:" << std::endl << e.what();
    }
    GTEST_SUCCEED();
}

/*
 * make instance of A and write it to file.
 * read file such that the type a is known but it's fields are unknown / lazy.
 * do nothing, write it and read it again.
 */
TEST(UnknownWriteTestA, make_nothing) {
    try {
        auto sf = common::tempFile<aWithFields::api::File>();
        sf->A->make();
        sf->close();

        std::unique_ptr<a::api::File> sf2(a::api::File::open(sf->currentPath()));
        ASSERT_EQ(1, sf2->A->size());
        sf2->close();

        std::unique_ptr<aWithFields::api::File> sf3(aWithFields::api::File::open(sf->currentPath()));
        ASSERT_EQ(1, sf3->A->size());

    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "an exception was thrown:" << std::endl << e.what();
    }
    GTEST_SUCCEED();
}

/*
 * write no instances to file.
 * read file such that the type a is known but it's fields are unknown / lazy.
 * make instance of a, write it and read it again.
 */
TEST(UnknownWriteTestA, nothing_make) {
    try {
        auto sf = common::tempFile<aWithFields::api::File>();
        sf->close();

        std::unique_ptr<a::api::File> sf2(a::api::File::open(sf->currentPath()));
        ASSERT_EQ(0, sf2->A->size());
        sf2->A->make();
        ASSERT_EQ(1, sf2->A->size());
        sf2->close();

        std::unique_ptr<aWithFields::api::File> sf3(aWithFields::api::File::open(sf->currentPath()));
        ASSERT_EQ(1, sf3->A->size());

    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "an exception was thrown:" << std::endl << e.what();
    }
    GTEST_SUCCEED();
}

/*
 * make instance of A and write it to file.
 * read file such that the type a is known but it's fields are unknown / lazy.
 * make instance of a, write it and read it again.
 */
TEST(UnknownWriteTestA, make_make) {
    try {
        auto sf = common::tempFile<aWithFields::api::File>();
        sf->A->make();
        sf->close();

        std::unique_ptr<a::api::File> sf2(a::api::File::open(sf->currentPath()));
        ASSERT_EQ(1, sf2->A->size());
        sf2->A->make();
        ASSERT_EQ(2, sf2->A->size());
        sf2->close();

        std::unique_ptr<aWithFields::api::File> sf3(aWithFields::api::File::open(sf->currentPath()));
        ASSERT_EQ(2, sf3->A->size());

    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "an exception was thrown:" << std::endl << e.what();
    }
    GTEST_SUCCEED();
}

//TODO mach mal was mit flush!
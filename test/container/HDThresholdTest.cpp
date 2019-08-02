#include <gtest/gtest.h>
#include "../common/utils.h"
#include "../../src/container/File.h"

using ::container::api::File;
using namespace common;

TEST(HD_Threshold_Test, equal) {
    try {
        auto sf = common::tempFile<File>();

        for (int i = 0; i < ::ogss::HD_Threshold; i++) {
            // create objects
            auto cont = sf->Container->make();
            // set fields
            cont->setL(put<int64_t>(array<int64_t>(), 0L));
        }
        sf->close();

        std::unique_ptr<File> sf2(File::open(sf->currentPath()));

        ASSERT_EQ(::ogss::HD_Threshold, sf2->Container->size());

        for (int i = 0; i < ::ogss::HD_Threshold; i++) {
            // create object from file
            auto cont = sf->Container->get(i+1);
            // assert fields
            ASSERT_NE(nullptr, cont->getL());
            ASSERT_EQ(1, cont->getL()->size());
            ASSERT_EQ(0, cont->getL()->get(0).i64);
        }

    } catch (ogss::Exception& e) {
        GTEST_FAIL() << "an exception was thrown:" << std::endl << e.what();
    }
    GTEST_SUCCEED();
}


TEST(HD_Threshold_Test, smaller) {
    try {
        auto sf = common::tempFile<File>();

        for (int i = 0; i < ::ogss::HD_Threshold-1; i++) {
            // create objects
            auto cont = sf->Container->make();
            // set fields
            cont->setL(put<int64_t>(array<int64_t>(), 0L));
        }
        sf->close();

        std::unique_ptr<File> sf2(File::open(sf->currentPath()));

        ASSERT_EQ(::ogss::HD_Threshold-1, sf2->Container->size());

        for (int i = 0; i < ::ogss::HD_Threshold-1; i++) {
            // create object from file
            auto cont = sf->Container->get(i+1);
            // assert fields
            ASSERT_NE(nullptr, cont->getL());
            ASSERT_EQ(1, cont->getL()->size());
            ASSERT_EQ(0, cont->getL()->get(0).i64);
        }

    } catch (ogss::Exception& e) {
        GTEST_FAIL() << "an exception was thrown:" << std::endl << e.what();
    }
    GTEST_SUCCEED();
}


TEST(HD_Threshold_Test, greater) {
    try {
        auto sf = common::tempFile<File>();

        for (int i = 0; i < ::ogss::HD_Threshold+1; i++) {
            // create objects
            auto cont = sf->Container->make();
            // set fields
            cont->setL(put<int64_t>(array<int64_t>(), 0L));
        }
        sf->close();

        std::unique_ptr<File> sf2(File::open(sf->currentPath()));

        ASSERT_EQ(::ogss::HD_Threshold+1, sf2->Container->size());

        for (int i = 0; i < ::ogss::HD_Threshold+1; i++) {
            // create object from file
            auto cont = sf->Container->get(i+1);
            // assert fields
            ASSERT_NE(nullptr, cont->getL());
            ASSERT_EQ(1, cont->getL()->size());
            ASSERT_EQ(0, cont->getL()->get(0).i64);
        }

    } catch (ogss::Exception& e) {
        GTEST_FAIL() << "an exception was thrown:" << std::endl << e.what();
    }
    GTEST_SUCCEED();
}


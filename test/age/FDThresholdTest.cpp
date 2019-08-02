#include <gtest/gtest.h>
#include "../common/utils.h"
#include "../../src/age/File.h"

using ::age::api::File;
using namespace common;

TEST(FD_Threshold_Test, equal) {
    try {
        auto sf = common::tempFile<File>();

        for (int i = 0; i < ::ogss::FD_Threshold; i++) {
            // create objects
            sf->Age->make()->setAge(42);
        }
        sf->close();

        std::unique_ptr<File> sf2(File::open(sf->currentPath()));

        ASSERT_EQ(::ogss::FD_Threshold, sf2->Age->size());

        for (int i = 0; i < ::ogss::FD_Threshold; i++) {
            // create object from file
            auto age = sf->Age->get(i+1);
            // assert fields
            ASSERT_NE(nullptr, age);
            ASSERT_EQ(42, age->getAge());
        }

    } catch (ogss::Exception& e) {
        GTEST_FAIL() << "an exception was thrown:" << std::endl << e.what();
    }
    GTEST_SUCCEED();
}

TEST(FD_Threshold_Test, smaller) {
    try {
        auto sf = common::tempFile<File>();

        for (int i = 0; i < ::ogss::FD_Threshold-1; i++) {
            // create objects
            sf->Age->make()->setAge(42);
        }
        sf->close();

        std::unique_ptr<File> sf2(File::open(sf->currentPath()));

        ASSERT_EQ(::ogss::FD_Threshold-1, sf2->Age->size());

        for (int i = 0; i < ::ogss::FD_Threshold-1; i++) {
            // create object from file
            auto age = sf->Age->get(i+1);
            // assert fields
            ASSERT_NE(nullptr, age);
            ASSERT_EQ(42, age->getAge());
        }

    } catch (ogss::Exception& e) {
        GTEST_FAIL() << "an exception was thrown:" << std::endl << e.what();
    }
    GTEST_SUCCEED();
}

TEST(FD_Threshold_Test, greater) {
    try {
        auto sf = common::tempFile<File>();

        for (int i = 0; i < ::ogss::FD_Threshold+1; i++) {
            // create objects
            sf->Age->make()->setAge(42);
        }
        sf->close();

        std::unique_ptr<File> sf2(File::open(sf->currentPath()));

        ASSERT_EQ(::ogss::FD_Threshold+1, sf2->Age->size());

        for (int i = 0; i < ::ogss::FD_Threshold+1; i++) {
            // create object from file
            auto age = sf->Age->get(i+1);
            // assert fields
            ASSERT_NE(nullptr, age);
            ASSERT_EQ(42, age->getAge());
        }

    } catch (ogss::Exception& e) {
        GTEST_FAIL() << "an exception was thrown:" << std::endl << e.what();
    }
    GTEST_SUCCEED();
}

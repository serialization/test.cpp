//
// Created by Timm Felden on 07.12.15.
//


#include <gtest/gtest.h>
#include "../../src/age/File.h"
#include "../common/utils.h"

using ::age::api::File;

TEST(AgeWriteTest, WriteAge) {
    try {
        auto sf = common::tempFile<File>();
        sf->Age->build()->age(1)->make();
        sf->Age->build()->age(28)->make();
        sf->close();

        ASSERT_EQ(2, sf->Age->size());
        ASSERT_EQ(1, sf->Age->get(1)->getAge());
        ASSERT_EQ(28, sf->Age->get(2)->getAge());

        auto sf2 = File::open(sf->currentPath());
        ASSERT_EQ(2, sf2->Age->size());
        ASSERT_EQ(1, sf2->Age->get(1)->getAge());
        ASSERT_EQ(28, sf2->Age->get(2)->getAge());
        delete sf2;
    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}

TEST(AgeWriteTest, DeleteFreshAge) {
    try {
        auto sf = common::tempFile<File>();
        auto a = sf->Age->build()->age(1)->make();
        sf->Age->build()->age(28)->make();

        sf->free(a);

        sf->close();

        //        ASSERT_EQ(1, sf->Age->size());
        //        ASSERT_EQ(28, sf->Age->get(1)->getAge());

        auto sf2 = File::open(sf->currentPath());
        ASSERT_EQ(1, sf2->Age->size());
        ASSERT_EQ(28, sf2->Age->get(1)->getAge());
        delete sf2;
    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}

TEST(AgeWriteTest, DeleteOldAge) {
    try {
        auto sf = common::tempFile<File>();
        sf->Age->build()->age(1)->make();
        sf->Age->build()->age(28)->make();
        sf->close();

        ASSERT_EQ(2, sf->Age->size());
        ASSERT_EQ(1, sf->Age->get(1)->getAge());
        ASSERT_EQ(28, sf->Age->get(2)->getAge());

        {
            auto sf2 = File::open(sf->currentPath());
            ASSERT_EQ(2, sf2->Age->size());
            ASSERT_EQ(1, sf2->Age->get(1)->getAge());
            ASSERT_EQ(28, sf2->Age->get(2)->getAge());
            sf2->free(sf2->Age->get(1));
            sf2->close();
            delete sf2;
        }
        {
            auto sf2 = File::open(sf->currentPath());
            ASSERT_EQ(1, sf2->Age->size());
            ASSERT_EQ(28, sf2->Age->get(1)->getAge());
            sf2->close();
            delete sf2;
        }
    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}
//
// Created by Sarah Stieß on 28.05.19.
//

#include "../../lib/ogss.common.cpp/ogss/internal/AbstractPool.h"
#include "../../src/age/File.h"
#include "../../src/basicTypes/File.h"
#include "../../src/empty/File.h"
#include "../common/utils.h"
#include <gtest/gtest.h>

using ::age::api::File;

TEST(AgePoolOfTest, PoolOfNull) {
    try {
        auto sg = common::tempFile<File>();

        ASSERT_EQ(nullptr, sg->pool(nullptr));

    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}

TEST(AgePoolOfTest, PoolOfCaseSensivity) {
    try {
        auto sg = common::tempFile<File>();

        std::string name("aGE");
        ASSERT_EQ(nullptr, sg->pool(&name));

    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}

TEST(AgePoolOfTest, PoolOfForeignObject) {
    try {
        auto sg = common::tempFile<File>();
        auto sg1 = common::tempFile<::basicTypes::api::File>();

        ASSERT_NE(nullptr, sg1->pool(sg->Age->make()));
        ASSERT_NE(nullptr, sg->pool(sg1->BasicBool->make()));

    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}

TEST(AgePoolOfTest, PoolOfForeignObject1) {
    try {
        auto sg = common::tempFile<File>();
        auto sg1 = common::tempFile<::empty::api::File>();

        ASSERT_FALSE(sg1->pool(sg->Age->make()));

    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}

TEST(AgePoolOfTest, PoolOfForeignObject1Contains) {
    try {
        auto sg = common::tempFile<File>();
        auto sg1 = common::tempFile<::empty::api::File>();

        ASSERT_FALSE(sg1->contains(sg->Age->make()));

    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}

TEST(AgePoolOfTest, PoolOfUnknownType) {
    try {
        auto sg = common::tempFile<File>();
        sg->Age->make();
        sg->close();

        auto sg1 = std::unique_ptr<::empty::api::File>(
          ::empty::api::File::open(sg->currentPath()));
        auto p = *sg1->begin();
        auto o = p->allObjects()->next();

        ASSERT_EQ(*(sg->Age->name), *(sg1->pool(o)->name));
        ASSERT_EQ(*(sg->Age->name), *(sg1->pool(sg->Age->name)->name));

    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}

TEST(AgePoolOfTest, PoolOfUnknownType2) {
    try {
        auto sg = common::tempFile<File>();
        age::Age *a = sg->Age->make();
        sg->close();

        auto sg1 = std::unique_ptr<::empty::api::File>(
          ::empty::api::File::open(sg->currentPath()));
        auto p = *sg1->begin();

        ASSERT_EQ(p, sg1->pool(p->allObjects()->next()));
        ASSERT_FALSE(sg1->pool(a));

    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}

TEST(AgePoolOfTest, PoolOfKnownType) {
    try {
        auto sg = common::tempFile<File>();
        auto age = sg->Age->make();
        sg->close();

        ASSERT_EQ(sg->Age, sg->pool(age));
        std::string name("Age");
        ASSERT_EQ(sg->Age, sg->pool(&name));
        ASSERT_EQ(sg->Age, sg->pool(sg->Age->name));

    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}

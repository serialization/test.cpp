//
// Created by Sarah Stieß on 28.05.19.
//

#include "../../lib/ogss.common.cpp/ogss/internal/AbstractPool.h"
#include "../../src/age/File.h"
#include "../../src/empty/File.h"
#include "../common/utils.h"
#include <gtest/gtest.h>

using ::age::api::File;

TEST(AgePoolAddTest, ContainsNull) {
    try {
        auto sg = common::tempFile<File>();
        ASSERT_FALSE(sg->contains(nullptr));
    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}

TEST(AgePoolAddTest, ContainsNew) {
    try {
        auto sg = common::tempFile<File>();
        auto age = sg->Age->make();
        ASSERT_TRUE(sg->contains(age));

    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}

TEST(AgePoolAddTest, ContainsKnown) {
    try {
        auto sg = common::tempFile<File>();
        sg->Age->build()->age(0)->make();
        sg->Age->build()->age(1)->make();
        sg->close();

        auto sg1 = std::unique_ptr<File>(File::open(sg->currentPath()));

        auto it = sg1->Age->allObjects();
        // just ensuring that we'll enter the loop.
        ASSERT_TRUE(it->hasNext());
        while (it->hasNext()) {
            auto x = it->next();
            ASSERT_TRUE(sg1->contains(x));
            ASSERT_FALSE(sg->contains(x));
        }

    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}
TEST(AgePoolAddTest, ContainsUnknown) {
    try {
        auto sg = common::tempFile<File>();
        sg->Age->build()->age(0)->make();
        sg->Age->build()->age(1)->make();
        sg->close();

        auto sg1 = std::unique_ptr<::empty::api::File>(
          ::empty::api::File::open(sg->currentPath()));

        // age type is the only type. empty has no types of its own.
        ASSERT_EQ(1, sg1->size());

        for (auto t = sg1->begin(); t < sg1->end(); t++) {
            ASSERT_EQ(2, (*t)->size());
            auto it = (*t)->allObjects();
            // just ensuring that we'll enter the loop.
            ASSERT_TRUE(it->hasNext());
            while (it->hasNext()) {
                auto x = it->next();
                ASSERT_TRUE(sg1->contains(x));
                ASSERT_FALSE(sg->contains(x));
            }
        }
    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}
TEST(AgePoolAddTest, ContainsForeign) {
    try {
        auto sg = common::tempFile<File>();
        sg->Age->build()->age(0)->make();
        sg->Age->build()->age(1)->make();
        sg->close();
        ASSERT_EQ(2, sg->Age->size());

        auto sg1 = common::tempFile<File>();
        ASSERT_EQ(0, sg1->Age->size());

        auto it = sg->Age->allObjects();
        // just ensuring that we'll enter the loop.
        ASSERT_TRUE(it->hasNext());
        while (it->hasNext()) {
            auto x = it->next();
            ASSERT_FALSE(sg1->contains(x));
        }

    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}
TEST(AgePoolAddTest, ContainsDeleted) {
    try {
        auto sg = common::tempFile<File>();
        auto age = sg->Age->build()->age(0)->make();
        sg->free(age);
        ASSERT_FALSE(sg->contains(age));
        sg->flush();
        ASSERT_FALSE(sg->contains(age));
    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}
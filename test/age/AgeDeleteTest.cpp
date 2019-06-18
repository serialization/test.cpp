//
// Created by Sarah Stieß on 28.05.19.
//

#include <gtest/gtest.h>
#include "../../src/age/File.h"
#include "../common/utils.h"
#include "../../src/empty/File.h"
#include "../../lib/ogss.common.cpp/ogss/internal/AbstractPool.h"

using ::age::api::File;

TEST(AgeDeleteTest, DeleteKnown) {
    try {
        auto sg = common::tempFile<File>();
        sg->Age->build()->age(0)->make();
        sg->Age->build()->age(1)->make();
        sg->close();

        // age does know age
        auto sg1 = std::unique_ptr<File>(File::open(sg->currentPath()));

        ASSERT_EQ(1, sg1->size());
        for (auto t = sg1->begin(); t < sg1->end(); t++) {
            ASSERT_EQ(2, (*t)->size());
            auto it = (*t)->allObjects();

            while (it->hasNext()) {
                sg1->free(it->next());
            }
            sg1->flush();
            ASSERT_EQ(0, (*t)->size());
        }
    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}

TEST(AgeDeleteTest, DeleteUnknown) {
    try {
        auto sg = common::tempFile<File>();
        sg->Age->build()->age(0)->make();
        sg->Age->build()->age(1)->make();
        sg->close();

        //empty does not know age
        auto sg1 = std::unique_ptr<::empty::api::File>(::empty::api::File::open(sg->currentPath()));

        ASSERT_EQ(1, sg1->size());

        for (auto t = sg1->begin(); t < sg1->end(); t++) {
            ASSERT_EQ(2, (*t)->size());
            auto it = (*t)->allObjects();

            while (it->hasNext()) {
                sg1->free(it->next());
            }
            sg1->flush();
            ASSERT_EQ(0, (*t)->size());
        }
    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}

TEST(AgeDeleteTest, DeletedCorrectInstance) {
    try {
        auto sg = common::tempFile<File>();
        auto age0 = sg->Age->build()->age(0)->make();
        auto age1 = sg->Age->build()->age(1)->make();
        ASSERT_EQ(2, sg->Age->size());

        sg->free(age0);
        sg->flush();
        ASSERT_EQ(1, sg->Age->size());
        ASSERT_EQ(age1, sg->Age->get(age1->ID()));

    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}

TEST(AgeDeleteTest, DeletedCorrectInstanceWithWrite) {
    try {
        auto sg = common::tempFile<File>();
        auto age0 = sg->Age->build()->age(0)->make();
        auto age1 = sg->Age->build()->age(1)->make();
        ASSERT_EQ(2, sg->Age->size());

        sg->free(age0);
        sg->flush();

        auto sg1 = std::unique_ptr<File>(File::open(sg->currentPath()));

        ASSERT_EQ(1, sg1->Age->size());
        ASSERT_EQ(1, sg1->Age->get(age1->ID())->getAge());

    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}
TEST(AgeDeleteTest, DeleteTwice) {
    try {
        auto sg = common::tempFile<File>();
        auto age0 = sg->Age->build()->age(0)->make();
        ASSERT_EQ(1, sg->Age->size());

        sg->free(age0);
        sg->flush();
        ASSERT_EQ(0, sg->Age->size());

        sg->free(age0);
        sg->flush();
        ASSERT_EQ(0, sg->Age->size());

    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}

TEST(AgeDeleteTest, DeleteNull) {
    try {
        auto sg = common::tempFile<File>();
        auto age0 = sg->Age->build()->age(0)->make();
        ASSERT_EQ(1, sg->Age->size());

        sg->free(nullptr);
        sg->close();
        ASSERT_EQ(1, sg->Age->size());

    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "unexpected failure: " << e.what();
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}

TEST(AgeDeleteTest, DeleteIllegal) {
    try {
        auto sg = common::tempFile<File>();
        auto sg1 = common::tempFile<File>();
        sg1->free(sg->Age->build()->age(0)->make());

        //sg1->close();

        GTEST_FAIL() << "illegal deletes should be checked for"; //sonst exception in close();
    } catch (ogss::Exception &e) {
        // everything all right
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}
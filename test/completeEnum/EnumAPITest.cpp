#include "../../src/completeEnum/File.h"
#include "../../src/incompleteEnum/File.h"
#include "../common/utils.h"
#include <gtest/gtest.h>

using ::completeEnum::api::File;
using namespace common;

TEST(CompleteEnum_API_Test, Builder) {
    try {
        auto sf = common::tempFile<File>();

        // create objects
        auto tst = sf->Typ->build()->brot(completeEnum::Brot::roggen)->make();

        // set fields

        sf->close();

        std::unique_ptr<File> sf2(File::open(sf->currentPath()));
        sf2->check();
    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "an exception was thrown:" << std::endl << e.what();
    }
    GTEST_SUCCEED();
}

TEST(CompleteEnum_API_Test, Proxy) {
    try {
        auto sf = common::tempFile<File>();

        // create objects
        auto tst = sf->Typ->make();

        ASSERT_TRUE(tst->getBrotProxy());

        ASSERT_EQ(completeEnum::Brot::roggen, tst->getBrot());

        sf->close();

        std::unique_ptr<File> sf2(File::open(sf->currentPath()));
        sf2->check();
    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "an exception was thrown:" << std::endl << e.what();
    }
    GTEST_SUCCEED();
}

TEST(CompleteEnum_API_Test, Init) {
    try {
        auto sf = common::tempFile<incompleteEnum::api::File>();
        // create objects
        sf->Typ->make();
        sf->close();

        std::unique_ptr<File> sf2(File::open(sf->currentPath()));
        sf2->check();

        auto t2 = sf2->Typ->get(1);

        ASSERT_TRUE(t2->getBrotProxy());
        ASSERT_EQ(completeEnum::Brot::roggen, t2->getBrot());
    } catch (ogss::Exception &e) {
        GTEST_FAIL() << "an exception was thrown:" << std::endl << e.what();
    }
    GTEST_SUCCEED();
}

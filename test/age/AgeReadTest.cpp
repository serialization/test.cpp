//
// Created by Timm Felden on 07.12.15.
//


#include <gtest/gtest.h>
#include <ogss/internal/UnknownObject.h>
#include "../../src/age/File.h"
#include "../../src/age/StringKeeper.h"

using ::age::api::File;

TEST(AgeReadTest, ReadAge) {
    auto sf = std::unique_ptr<File>(
            File::open("../../src/test/resources/binarygen/[[all]]/accept/twoAges.sg"));
    ASSERT_EQ(2, sf->Age->size());
    for (auto i = sf->Age->size(); i > 0; i--) {
        sf->Age->get(i);
    }
    ASSERT_EQ(nullptr, sf->Age->get(0));
    ASSERT_EQ(30, sf->Age->get(1)->getAge());
    ASSERT_EQ(2, sf->Age->get(2)->getAge());
}

TEST(AgeReadTest, ReadAgeForachAPI) {
    auto sf = std::unique_ptr<File>(
            File::open("../../src/test/resources/binarygen/[[all]]/accept/twoAges.sg"));
    ASSERT_EQ(2, sf->Age->size());
    const char *as = "\x1e\x02";
    for (auto &age : *sf->Age) {
        ASSERT_EQ(*as++, age.getAge()) << "found wrong age";
    }
    ASSERT_EQ(0, *as) << "less or more as then expected";
}

TEST(AgeReadTest, ReadAgeForachIterator) {
    auto sf = std::unique_ptr<File>(
            File::open("../../src/test/resources/binarygen/[[all]]/accept/twoAges.sg"));
    ASSERT_EQ(2, sf->Age->size());
    const char *as = "\x1e\x02";
    for (const auto &age : sf->Age->all()) {
        ASSERT_EQ(*as++, age.getAge()) << "found wrong age";
    }
    ASSERT_EQ(0, *as) << "less or more as then expected";
}

TEST(AgeReadTest, ReadAgeForachIteratorOverwrite) {
    auto sf = std::unique_ptr<File>(
            File::open("../../src/test/resources/binarygen/[[all]]/accept/twoAges.sg"));
    ASSERT_EQ(2, sf->Age->size());
    // set new ages
    const char *as = "\x10\x11";
    const char *p = as;
    for (auto &age : sf->Age->all()) {
        age.setAge(*p++);
    }
    for (const auto &age : sf->Age->all()) {
        ASSERT_EQ(*as++, age.getAge()) << "found wrong age";
    }
    ASSERT_EQ(0, *as) << "less or more as then expected";
}

TEST(AgeReadTest, ReadAgeWhileIterator) {
    auto sf = std::unique_ptr<File>(
            File::open("../../src/test/resources/binarygen/[[all]]/accept/twoAges.sg"));
    ASSERT_EQ(2, sf->Age->size());
    const char *as = "\x1e\x02";

    auto vs = sf->Age->begin();
    while (vs.hasNext()) {
        auto v = vs.next();
        ASSERT_EQ(*as++, v->getAge()) << "found wrong age";
    }
    ASSERT_EQ(0, *as) << "less or more as then expected";
}

TEST(AgeReadTest, ReadAgeCheckTypes) {
    auto sf = std::unique_ptr<File>(
            File::open("../../src/test/resources/binarygen/[[all]]/accept/twoAges.sg"));
    ASSERT_EQ(1, (int) sf->size());
    for (auto t : *sf) {
        ASSERT_EQ(age::internal::SK.Age, t->name);
    }
}

TEST(AgeReadTest, ReadAgeCheckInstanceOrder) {
    auto sf = std::unique_ptr<File>(
            File::open("../../src/test/resources/binarygen/[[all]]/accept/twoAges.sg"));
    ASSERT_EQ(1, (int) sf->size());
    auto as = sf->Age->allInTypeOrder();
    for (ogss::ObjectID i = 1; i <= sf->Age->size(); i++) {
        ASSERT_EQ(as.next()->ID(), i);
    }
}

TEST(AgeReadTest, CheckLBPOTypeHierarchyOrder) {

    auto checkType = [](ogss::internal::AbstractPool *t, const char *name,
                        const char *types) -> void {
        if (*t->name == std::string(name)) {
            ogss::iterators::TypeHierarchyIterator ts(t);
            for (auto &p : ts)
                ASSERT_EQ(*types++, p.name->at(0))
                                            << name << " contained wrong instances";
            ASSERT_EQ(0, *types) << name << " is not at end of string";
        }
    };

    auto sf = std::unique_ptr<File>(
            File::open("../../src/test/resources/binarygen/[[empty]]/accept/poly.sg"));
    ASSERT_EQ(5, (int) sf->size());
    for (auto t : *sf) {
        checkType(t, "a", "abdc");
        checkType(t, "b", "bd");
        checkType(t, "c", "c");
        checkType(t, "d", "d");
    }
}

TEST(AgeReadTest, ReadLBPOCheckTypeOrder) {
    typedef ::ogss::internal::Pool<ogss::internal::UnknownObject> *uPool;

    auto checkType = [](ogss::internal::AbstractPool *t, const char *name,
                        const char *types) -> void {
        if (*t->name == std::string(name)) {
            uPool pool = (uPool) t;
            const auto &is = pool->allInTypeOrder();
            for (auto &i : is)
                ASSERT_EQ(*types++, i.pool->name->at(0))
                                            << name << " contained wrong instances";

            ASSERT_EQ(0, *types) << name << " is not at end of string";
        }
    };

    auto sf = std::unique_ptr<File>(
            File::open("../../src/test/resources/binarygen/[[empty]]/accept/poly.sg"));
    ASSERT_EQ(5, (int) sf->size());
    for (auto t : *sf) {
        checkType(t, "a", "aaabbbbbdddcc");
        checkType(t, "b", "bbbbbddd");
        checkType(t, "c", "cc");
        checkType(t, "d", "ddd");
    }
}

TEST(AgeReadTest, ReadLBPOCheckStaticInstances) {
    typedef ::ogss::internal::Pool<ogss::internal::UnknownObject> *uPool;

    auto checkType = [](ogss::internal::AbstractPool *t, const char *name,
                        const char *types) -> void {
        if (*t->name == std::string(name)) {
            uPool pool = (uPool) t;
            for (auto &i : pool->staticInstances())
                ASSERT_EQ(*types++, ((age::Age &) i).ID())
                                            << name << " contained wrong instances";

            ASSERT_EQ(0, *types) << name << " is not at end of string";
        }
    };

    auto sf = std::unique_ptr<File>(
            File::open("../../src/test/resources/binarygen/[[empty]]/accept/poly.sg"));
    ASSERT_EQ(5, (int) sf->size());
    for (auto t : *sf) {
        // TODO indices are likely wrong, because the SKilL block layout is gone
        checkType(t, "a", "\x01\x02\x0B");
        checkType(t, "b", "\x03\x04\x05\x07\x08");
        checkType(t, "c", "\x06\x0D");
        checkType(t, "d", "\x09\x0A\x0C");
    }
}

TEST(AgeReadTest, ReadLBPOCheckAllocationOrder) {
    typedef ::ogss::internal::Pool<ogss::internal::UnknownObject> *uPool;

    auto checkType = [](ogss::internal::AbstractPool *t, const char *name,
                        const char *types) -> void {
        if (*t->name == std::string(name)) {
            uPool pool = (uPool) t;
            for (auto &i : pool->all())
                ASSERT_EQ(*types++, i.pool->name->at(0))
                                            << name << " contained wrong instances";


            ASSERT_EQ(0, *types) << name << " is not at end of string";
        }
    };

    auto sf = std::unique_ptr<File>(
            File::open("../../src/test/resources/binarygen/[[empty]]/accept/poly.sg"));
    ASSERT_EQ(5, (int) sf->size());
    for (auto t : *sf) {
        checkType(t, "a", "aabbbcbbddadc");
        checkType(t, "b", "bbbbbddd");
        checkType(t, "c", "cc");
        checkType(t, "d", "ddd");
    }
}

TEST(AgeReadTest, ReadDate) {
    auto sf = std::unique_ptr<File>(
            File::open("../../src/test/resources/binarygen/[[all]]/accept/numbers.sg"));
    ASSERT_NE(nullptr, sf->Age);
    ASSERT_EQ(0, (int) sf->Age->size());
    for (auto i = sf->Age->size(); i > 0; i--) {
        sf->Age->get(i);
    }
    ASSERT_EQ(nullptr, sf->Age->get(0));
}

TEST(AgeReadTest, ReadWriteDate) {
    auto sf = std::unique_ptr<File>(
            File::open("../../src/test/resources/binarygen/[[all]]/accept/numbers.sg"));
    ASSERT_NE(nullptr, sf->Age);
    ASSERT_EQ(0, (int) sf->Age->size());
    sf->check();
    sf->changePath("/tmp/out.sf");
    sf->close();
}
#include <gtest/gtest.h>

#include "Meta/metalib.h"

#include "Meta/serialize/container/syncablecontainer.h"
#include "Meta/serialize/serializableunit.h"

#include "Meta/serialize/streams/serializestreambuf.h"

#include "Meta/serialize/toplevelunit.h"

#include "Meta/serialize/container/noparent.h"

#include "Meta/serialize/serializetable_impl.h"

#include "../testManager.h"

using namespace Engine::Serialize;
using namespace std::chrono_literals;

struct TestUnit : TopLevelUnit<TestUnit> {
    SERIALIZABLEUNIT(TestUnit);

    TestUnit()
        : TopLevelUnit<TestUnit>(10)
    {
    }

    SERIALIZABLE_CONTAINER(list1, std::list<int>);
    SERIALIZABLE_CONTAINER(set1, std::set<int>);
    SYNCABLE_CONTAINER(list2, std::list<int>);
    SYNCABLE_CONTAINER(set2, std::set<int>);
};

SERIALIZETABLE_BEGIN(TestUnit)
FIELD(list1)
FIELD(list2)
FIELD(set1)
FIELD(set2)
SERIALIZETABLE_END(TestUnit)

TEST(Serialize_Container, SyncedUnit)
{

    TestManager mgr1("container1");
    TestManager mgr2("container2");

    NoParentUnit<TestUnit> unit1;
    NoParentUnit<TestUnit> unit2;

    unit1.list1.emplace(unit1.list1.end(), 3);
    unit1.list1.emplace(unit1.list1.end(), 4);
    unit1.list1.emplace(unit1.list1.end(), 5);

    unit1.list2.emplace(unit1.list2.end(), 1);
    unit1.list2.emplace(unit1.list2.end(), 2);

    unit1.set1 = { 1, 2, 3 };
    unit2.set1 = { 4, 5, 6 };

    ASSERT_TRUE(mgr1.addTopLevelItem(&unit1));
    ASSERT_TRUE(mgr2.addTopLevelItem(&unit2));

    Buffer buffer;
    mgr1.setBuffer(buffer, false);
    mgr1.sendMessages();
    mgr2.setBuffer(buffer, true);

    ASSERT_EQ(unit1.list1, unit2.list1);
    ASSERT_EQ(unit1.list2, unit2.list2);
    ASSERT_EQ(unit1.set1, unit2.set1);

    bool called = false;
    unit1.list2.emplace(unit1.list2.end(), 6).onSuccess([&](auto &&pib) {
        called = true; 
    });
    ASSERT_TRUE(called);
    ASSERT_EQ(unit1.list2.back(), 6);

    mgr1.sendMessages();
    mgr2.receiveMessages(1, 0ms);    

    ASSERT_EQ(unit1.list2, unit2.list2);

    called = false;
    unit2.list2.emplace(unit2.list2.end(), 7).onSuccess([&](auto &&pib) { called = true; });

    ASSERT_EQ(unit1.list2, unit2.list2);

    mgr2.sendMessages();
    mgr1.receiveMessages(1, 0ms);

    ASSERT_EQ(unit1.list2.back(), 7);

    mgr1.sendMessages();
    ASSERT_FALSE(called);
    mgr2.receiveMessages(1, 0ms);
    ASSERT_TRUE(called);

    ASSERT_EQ(unit1.list2, unit2.list2);

    called = false;
    unit1.list2.erase(std::next(unit1.list2.begin())).onSuccess([&](auto &&pib) { called = true; });
    ASSERT_TRUE(called);

    ASSERT_EQ(unit1.list2.size(), 3);

    mgr1.sendMessages();
    mgr2.receiveMessages(1, 0ms);

    ASSERT_EQ(unit2.list2.size(), 3);
    ASSERT_EQ(unit1.list2, unit2.list2);

}


TEST(Serialize_Container, Array)
{

    //Engine::Threading::WorkGroup wg;

    std::array<uint32_t, 128> array;
    for (size_t i = 0; i < 128; ++i)
        array[i] = 2 * i;

    Buffer buffer;
    SerializeOutStream stream1 {
        std::make_unique<TestBuf>(buffer, true), std::make_unique<SerializeStreamData>(std::make_unique<SafeBinaryFormatter>())
    };
    SerializeInStream stream2 {
        std::make_unique<TestBuf>(buffer, false), std::make_unique<SerializeStreamData>(std::make_unique<SafeBinaryFormatter>())
    };

    stream1 << array;

    std::array<uint32_t, 128> array2;
    stream2 >> array2;

    ASSERT_EQ(array, array2);
}

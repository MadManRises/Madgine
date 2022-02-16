#include <gtest/gtest.h>

#include "Meta/metalib.h"

#include "Meta/serialize/serializableunit.h"

#include "Meta/serialize/streams/serializestreamdata.h"

#include "Meta/serialize/container/noparent.h"

#include "../testManager.h"
#include "../testunit.h"

#include "Meta/serialize/streams/operations.h"

using namespace Engine::Serialize;
using namespace std::chrono_literals;

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
    HANDLE_MGR_RESULT(mgr1, mgr1.setBuffer(buffer, false));
    mgr1.sendMessages();
    HANDLE_MGR_RESULT(mgr2, mgr2.setBuffer(buffer, true));

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
    FormattedSerializeStream stream1 {
        std::make_unique<SafeBinaryFormatter>(), SerializeStream { std::make_unique<TestBuf>(buffer, true) }
    };
    FormattedSerializeStream stream2 {
        std::make_unique<SafeBinaryFormatter>(), SerializeStream { std::make_unique<TestBuf>(buffer, false) }
    };

    write(stream1, array, "array");

    std::array<uint32_t, 128> array2;
    HANDLE_STREAM_RESULT(read(stream2, array2, nullptr));

    ASSERT_EQ(array, array2);
}

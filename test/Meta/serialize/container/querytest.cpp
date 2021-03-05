#include <gtest/gtest.h>

#include "Meta/metalib.h"

#include "Meta/serialize/serializableunit.h"

#include "Meta/serialize/container/noparent.h"

#include "../testManager.h"
#include "../testunit.h"

using namespace Engine::Serialize;
using namespace std::chrono_literals;


TEST(Serialize_Query, Query)
{

    TestManager mgr1("container1");
    TestManager mgr2("container2");

    NoParentUnit<TestUnit> unit1 { 1 };
    NoParentUnit<TestUnit> unit2 { 2 };

    ASSERT_TRUE(mgr1.addTopLevelItem(&unit1));
    ASSERT_TRUE(mgr2.addTopLevelItem(&unit2));

    Buffer buffer;
    mgr1.setBuffer(buffer, false);
    mgr1.sendMessages();
    mgr2.setBuffer(buffer, true);

    ASSERT_EQ(unit1.mCallCount, 0);
    ASSERT_EQ(unit2.mCallCount, 0);

    Engine::Future<int> f1 = unit1.query(10);

    ASSERT_TRUE(f1.is_ready());
    ASSERT_EQ(f1.get(), 11);

    ASSERT_EQ(unit1.mCallCount, 1);
    ASSERT_EQ(unit2.mCallCount, 0);

    mgr1.sendMessages();
    mgr2.receiveMessages(1, 0ms);

    ASSERT_EQ(unit1.mCallCount, 1);
    ASSERT_EQ(unit2.mCallCount, 0);

    Engine::Future<int> f2 = unit2.query(20);

    ASSERT_FALSE(f2.is_ready());

    ASSERT_EQ(unit1.mCallCount, 1);
    ASSERT_EQ(unit2.mCallCount, 0);

    mgr2.sendMessages();
    mgr1.receiveMessages(1, 0ms);

    ASSERT_FALSE(f2.is_ready());

    ASSERT_EQ(unit1.mCallCount, 2);
    ASSERT_EQ(unit2.mCallCount, 0);

    mgr1.sendMessages();
    mgr2.receiveMessages(1, 0ms);

    ASSERT_TRUE(f2.is_ready());
    ASSERT_EQ(f2.get(), 21);

    ASSERT_EQ(unit1.mCallCount, 2);
    ASSERT_EQ(unit2.mCallCount, 0);
}

TEST(Serialize_Query, Query_Hierarchical)
{

    TestManager mgr1("container1");
    TestManager mgr2("container2");    
    TestManager mgr3("container3");

    NoParentUnit<TestUnit> unit1 { 1 };
    NoParentUnit<TestUnit> unit2 { 2 };
    NoParentUnit<TestUnit> unit3 { 3 };

    ASSERT_TRUE(mgr1.addTopLevelItem(&unit1));
    ASSERT_TRUE(mgr2.addTopLevelItem(&unit2));
    ASSERT_TRUE(mgr3.addTopLevelItem(&unit3));

    Buffer buffer;
    mgr1.setBuffer(buffer, false);
    mgr1.sendMessages();
    mgr2.setBuffer(buffer, true);

    Buffer buffer2;
    mgr2.setBuffer(buffer2, false);
    mgr2.sendMessages();
    mgr3.setBuffer(buffer2, true);

    ASSERT_EQ(unit1.mCallCount, 0);
    ASSERT_EQ(unit2.mCallCount, 0);
    ASSERT_EQ(unit3.mCallCount, 0);

    Engine::Future<int> f1 = unit1.query(10);

    ASSERT_TRUE(f1.is_ready());
    ASSERT_EQ(f1.get(), 11);

    ASSERT_EQ(unit1.mCallCount, 1);
    ASSERT_EQ(unit2.mCallCount, 0);
    ASSERT_EQ(unit3.mCallCount, 0);

    mgr1.sendMessages();
    mgr2.receiveMessages(1, 0ms);

    ASSERT_EQ(unit1.mCallCount, 1);
    ASSERT_EQ(unit2.mCallCount, 0);
    ASSERT_EQ(unit3.mCallCount, 0);

    mgr2.sendMessages();
    mgr3.receiveMessages(1, 0ms);

    ASSERT_EQ(unit1.mCallCount, 1);
    ASSERT_EQ(unit2.mCallCount, 0);
    ASSERT_EQ(unit3.mCallCount, 0);

    Engine::Future<int> f2 = unit3.query(20);

    ASSERT_FALSE(f2.is_ready());

    ASSERT_EQ(unit1.mCallCount, 1);
    ASSERT_EQ(unit2.mCallCount, 0);
    ASSERT_EQ(unit3.mCallCount, 0);

    mgr3.sendMessages();
    mgr2.receiveMessages(1, 0ms);

    ASSERT_FALSE(f2.is_ready());

    ASSERT_EQ(unit1.mCallCount, 1);
    ASSERT_EQ(unit2.mCallCount, 0);
    ASSERT_EQ(unit3.mCallCount, 0);

    mgr2.sendMessages();
    mgr1.receiveMessages(1, 0ms);

    ASSERT_FALSE(f2.is_ready());

    ASSERT_EQ(unit1.mCallCount, 2);
    ASSERT_EQ(unit2.mCallCount, 0);
    ASSERT_EQ(unit3.mCallCount, 0);

    mgr1.sendMessages();
    mgr2.receiveMessages(1, 0ms);

    ASSERT_FALSE(f2.is_ready());

    ASSERT_EQ(unit1.mCallCount, 2);
    ASSERT_EQ(unit2.mCallCount, 0);
    ASSERT_EQ(unit3.mCallCount, 0);

    mgr2.sendMessages();
    mgr3.receiveMessages(1, 0ms);

    ASSERT_TRUE(f2.is_ready());
    ASSERT_EQ(f2.get(), 21);

    ASSERT_EQ(unit1.mCallCount, 2);
    ASSERT_EQ(unit2.mCallCount, 0);
    ASSERT_EQ(unit3.mCallCount, 0);
}
#include <gtest/gtest.h>

#include "Meta/metalib.h"

#include "Meta/serialize/container/query.h"
#include "Meta/serialize/serializableunit.h"

#include "Meta/serialize/toplevelunit.h"

#include "Meta/serialize/container/noparent.h"

#include "Meta/serialize/serializetable_impl.h"

#include "../testManager.h"

using namespace Engine::Serialize;
using namespace std::chrono_literals;

struct QueryTestUnit : TopLevelUnit<QueryTestUnit> {
    SERIALIZABLEUNIT(QueryTestUnit);

    QueryTestUnit(int data)
        : TopLevelUnit<QueryTestUnit>(10)
        , mData(data)
    {
    }

    int fooImpl(int i)
    {
        ++mCallCount;
        return i + mData;
    }

    int mCallCount = 0;
    int mData;

    QUERY(foo, fooImpl);
};

SERIALIZETABLE_BEGIN(QueryTestUnit)
SYNC(foo)
SERIALIZETABLE_END(QueryTestUnit)

TEST(Serialize_Query, Query)
{

    TestManager mgr1("container1");
    TestManager mgr2("container2");

    NoParentUnit<QueryTestUnit> unit1 { 1 };
    NoParentUnit<QueryTestUnit> unit2 { 2 };

    ASSERT_TRUE(mgr1.addTopLevelItem(&unit1));
    ASSERT_TRUE(mgr2.addTopLevelItem(&unit2));

    Buffer buffer;
    mgr1.setBuffer(buffer, false);
    mgr1.sendMessages();
    mgr2.setBuffer(buffer, true);

    ASSERT_EQ(unit1.mCallCount, 0);
    ASSERT_EQ(unit2.mCallCount, 0);

    Engine::Future<int> f1 = unit1.foo(10);

    ASSERT_TRUE(f1.isAvailable());
    ASSERT_EQ(f1.get(), 11);

    ASSERT_EQ(unit1.mCallCount, 1);
    ASSERT_EQ(unit2.mCallCount, 0);

    mgr1.sendMessages();
    mgr2.receiveMessages(1, 0ms);

    ASSERT_EQ(unit1.mCallCount, 1);
    ASSERT_EQ(unit2.mCallCount, 0);

    Engine::Future<int> f2 = unit2.foo(20);

    ASSERT_FALSE(f2.isAvailable());

    ASSERT_EQ(unit1.mCallCount, 1);
    ASSERT_EQ(unit2.mCallCount, 0);

    mgr2.sendMessages();
    mgr1.receiveMessages(1, 0ms);

    ASSERT_FALSE(f2.isAvailable());

    ASSERT_EQ(unit1.mCallCount, 2);
    ASSERT_EQ(unit2.mCallCount, 0);

    mgr1.sendMessages();
    mgr2.receiveMessages(1, 0ms);

    ASSERT_TRUE(f2.isAvailable());
    ASSERT_EQ(f2.get(), 21);

    ASSERT_EQ(unit1.mCallCount, 2);
    ASSERT_EQ(unit2.mCallCount, 0);
}

TEST(Serialize_Query, Query_Hierarchical)
{

    TestManager mgr1("container1");
    TestManager mgr2("container2");    
    TestManager mgr3("container3");

    NoParentUnit<QueryTestUnit> unit1 { 1 };
    NoParentUnit<QueryTestUnit> unit2 { 2 };
    NoParentUnit<QueryTestUnit> unit3 { 3 };

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

    Engine::Future<int> f1 = unit1.foo(10);

    ASSERT_TRUE(f1.isAvailable());
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

    Engine::Future<int> f2 = unit3.foo(20);

    ASSERT_FALSE(f2.isAvailable());

    ASSERT_EQ(unit1.mCallCount, 1);
    ASSERT_EQ(unit2.mCallCount, 0);
    ASSERT_EQ(unit3.mCallCount, 0);

    mgr3.sendMessages();
    mgr2.receiveMessages(1, 0ms);

    ASSERT_FALSE(f2.isAvailable());

    ASSERT_EQ(unit1.mCallCount, 1);
    ASSERT_EQ(unit2.mCallCount, 0);
    ASSERT_EQ(unit3.mCallCount, 0);

    mgr2.sendMessages();
    mgr1.receiveMessages(1, 0ms);

    ASSERT_FALSE(f2.isAvailable());

    ASSERT_EQ(unit1.mCallCount, 2);
    ASSERT_EQ(unit2.mCallCount, 0);
    ASSERT_EQ(unit3.mCallCount, 0);

    mgr1.sendMessages();
    mgr2.receiveMessages(1, 0ms);

    ASSERT_FALSE(f2.isAvailable());

    ASSERT_EQ(unit1.mCallCount, 2);
    ASSERT_EQ(unit2.mCallCount, 0);
    ASSERT_EQ(unit3.mCallCount, 0);

    mgr2.sendMessages();
    mgr3.receiveMessages(1, 0ms);

    ASSERT_TRUE(f2.isAvailable());
    ASSERT_EQ(f2.get(), 21);

    ASSERT_EQ(unit1.mCallCount, 2);
    ASSERT_EQ(unit2.mCallCount, 0);
    ASSERT_EQ(unit3.mCallCount, 0);
}

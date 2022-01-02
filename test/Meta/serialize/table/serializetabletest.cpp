#include <gtest/gtest.h>

#include "Meta/metalib.h"

#include "Meta/serialize/serializableunit.h"
#include "Meta/serialize/serializetable_impl.h"

#include "../testManager.h"

using namespace Engine::Serialize;
using namespace std::chrono_literals;

struct S : SerializableUnitBase {
    int j;
};

SERIALIZETABLE_BEGIN(S)
FIELD(j)
SERIALIZETABLE_END(S)

struct TestStruct : SerializableUnitBase {

    int i;
    S s;
};

SERIALIZETABLE_BEGIN(TestStruct)
FIELD(i)
FIELD(s)
SERIALIZETABLE_END(TestStruct)

TEST(Serialize_Table, Test1)
{

    TestManager mgr1("table1");
    TestManager mgr2("table2");


    Buffer buffer;
    HANDLE_MGR_RESULT(mgr1, mgr1.setBuffer(buffer, false, false));
    BufferedInOutStream &stream1 = mgr1.getMasterStream(1);
    HANDLE_MGR_RESULT(mgr2, mgr2.setBuffer(buffer, true, false));
    BufferedInOutStream &stream2 = *mgr2.getSlaveStream();

    TestStruct t1;
    t1.i = 1;
    t1.s.j = 2;

    serializeTable<TestStruct>().writeState(&t1, stream1);
    stream1.endMessage();
    stream1.sendMessages();

    TestStruct t2;

    stream2.isMessageAvailable();
    serializeTable<TestStruct>().readState(&t2, stream2);

    ASSERT_EQ(t1.i, t2.i);
}

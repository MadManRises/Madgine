#include <gtest/gtest.h>

#include "Modules/moduleslib.h"

#include "Modules/serialize/serializableunit.h"
#include "Modules/serialize/serializetable_impl.h"

#include "../testManager.h"

using namespace Engine::Serialize;
using namespace std::chrono_literals;

struct S : SerializableUnit<S> {
    int j;
};

SERIALIZETABLE_BEGIN(S)
FIELD(j)
SERIALIZETABLE_END(S)

struct TestStruct : SerializableUnit<TestStruct> {

	int i;
    S s;
	
};

SERIALIZETABLE_BEGIN(TestStruct)
FIELD(i)
FIELD(s)
SERIALIZETABLE_END(TestStruct)

TEST(Serialize_Table, Test1)
{

		Engine::Serialize::Debugging::setLoggingEnabled(true);

    TestManager mgr1("table1");
    TestManager mgr2("table2");


    Buffer buffer;
    BufferedInOutStream &stream1 = mgr1.setBuffer(buffer, false, false);
    BufferedInOutStream &stream2 = mgr2.setBuffer(buffer, true, false);

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

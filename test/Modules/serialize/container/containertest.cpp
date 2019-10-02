#include <gtest/gtest.h>

#include "Modules/moduleslib.h"

#include "Modules/serialize/container/syncablecontainer.h"
#include "Modules/serialize/serializableunit.h"

#include "Modules/threading/workgroup.h"


#include "Modules/serialize/toplevelserializableunit.h"

#include "Modules/serialize/container/noparent.h"



#include "Modules/serialize/container/offset.h"

#include "Modules/serialize/serializetable_impl.h"

#include "../testManager.h"

	using namespace Engine::Serialize;
	using namespace std::chrono_literals;




struct TestUnit : TopLevelSerializableUnit<TestUnit> {
    TestUnit()
        : TopLevelSerializableUnit<TestUnit>(10)
    {
    }

	SERIALIZABLE_CONTAINER(list1, std::list<int>);
    SYNCABLE_CONTAINER(list2, std::list<int>, ContainerPolicies::allowAll);
};

SERIALIZETABLE_BEGIN(TestUnit)
FIELD(list1)
FIELD(list2)
SERIALIZETABLE_END(TestUnit)

TEST(Serialize_Container, Test1)
{

		Engine::Serialize::Debugging::setLoggingEnabled(true);


	Engine::Threading::WorkGroup wg;

	TestManager mgr1("container1");
	TestManager mgr2("container2");


	NoParentUnit<TestUnit> unit1;
	NoParentUnit<TestUnit> unit2;

	unit1.list1.push_back(3);
	unit1.list1.push_back(4);
	unit1.list1.push_back(5);


	unit1.list2.push_back(1);
	unit1.list2.push_back(2);

	ASSERT_TRUE(mgr1.addTopLevelItem(&unit1));
	ASSERT_TRUE(mgr2.addTopLevelItem(&unit2));

	Buffer buffer;
	mgr1.setBuffer(buffer, false);
	mgr1.sendMessages();
	mgr2.setBuffer(buffer, true);

	ASSERT_EQ(unit1.list1, unit2.list1);
	ASSERT_EQ(unit1.list2, unit2.list2);

	unit1.list2.push_back(6);
	ASSERT_EQ(unit1.list2.back(), 6);

	mgr1.sendMessages();
	mgr2.receiveMessages(1, 1000ms);

	ASSERT_EQ(unit1.list2, unit2.list2);

	unit2.list2.push_back(7);

	ASSERT_EQ(unit1.list2, unit2.list2);

	mgr2.sendMessages();
	mgr1.receiveMessages(1, 1000ms);

	ASSERT_EQ(unit1.list2.back(), 7);

	mgr1.sendMessages();
	mgr2.receiveMessages(1, 1000ms);

	ASSERT_EQ(unit1.list2, unit2.list2);
}

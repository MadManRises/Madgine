#include <gtest/gtest.h>

#include "Interfaces/interfaceslib.h"

#include "Interfaces/serialize/container/list.h"
#include "Interfaces/serialize/serializableunit.h"

#include "Interfaces/threading/workgroup.h"

TEST(Serialize_Container, Test1)
{
	using namespace Engine::Serialize;

	Engine::Threading::WorkGroup wg;

	SerializableList<int> list;
	list.push_back(3);
	list.push_back(4);
	list.push_back(5);

	class TestUnit : public SerializableUnit<TestUnit>
	{
	public:
		ObservableList<int, ContainerPolicies::allowAll> list;
	};
	TestUnit unit;

	ObservableList<int, ContainerPolicies::allowAll>& list2 = unit.list;

	list2.push_back(1);
	list2.push_back(2);
}

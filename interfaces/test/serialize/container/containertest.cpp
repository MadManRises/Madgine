#include <gtest/gtest.h>

#include "interfaceslib.h"

#include "serialize/container/list.h"
#include "serialize/serializableunit.h"

TEST(Serialize_Container, Test1) {

	using namespace Engine::Serialize;

	SerializableList<int> list;
	list.push_back(3);
	list.push_back(4);
	list.push_back(5);

	class TestUnit : public SerializableUnit<TestUnit> {
	public:
		ObservableList<int, ContainerPolicy::allowAll> list;
	};
	TestUnit unit;

	ObservableList<int, ContainerPolicy::allowAll> &list2 = unit.list;
	
	list2.push_back(1);
	list2.push_back(2);

}

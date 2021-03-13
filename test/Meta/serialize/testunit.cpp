#include "Meta/metalib.h"

#include "testunit.h"

#include "Meta/serialize/serializetable_impl.h"

#include "Meta/serialize/container/action_operations.h"

#include "Meta/serialize/container/query_operations.h"

#include "Meta/serialize/configs/creator.h"

SERIALIZETABLE_BEGIN(ComplexDataType)
SERIALIZETABLE_END(ComplexDataType)

constexpr const char *list1ArgNames(size_t index)
{
    constexpr std::array<const char *, 4> names = {
        "first",
        "second",
        "third",
        "fourth"
    };
    return names[index];
}

std::tuple<int, float, std::string, bool> writeList1(const ComplexDataType &data)
{
    return {
        data.i,
        data.f,
        data.s,
        data.b
    };
}
std::tuple<int, float, std::string, bool> readList1(int i, float f, std::string s, bool b)
{
    return { i, f, s, b };
}

SERIALIZETABLE_BEGIN(TestUnit)
SYNC(action)
SYNC(query)

FIELD(list1)
FIELD(list2)
FIELD(set1)
FIELD(set2)
FIELD(map1, KeyValueCreator<DefaultCreator<const std::pair<int, float>>, DefaultCreator<int>>)

FIELD(complexList1, CustomCreator<&list1ArgNames, &readList1, &writeList1>)
FIELD(complexList2)
FIELD(complexList3)

FIELD(pod)

SERIALIZETABLE_END(TestUnit)

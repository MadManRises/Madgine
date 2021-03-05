#include "Meta/metalib.h"

#include "testunit.h"

#include "Meta/serialize/serializetable_impl.h"

#include "Meta/serialize/container/action_operations.h"

#include "Meta/serialize/container/query_operations.h"

#include "Meta/serialize/configs/creator.h"

SERIALIZETABLE_BEGIN(ComplexDataType)
SERIALIZETABLE_END(ComplexDataType)

std::tuple<std::pair<const char *, int>, std::pair<const char *, float>, std::pair<const char *, std::string>> writeList1(const ComplexDataType &data)
{
    return {
        { "first", data.i },
        { "second", data.f },
        { "third", data.s }
    };
}
std::tuple<int, float, std::string> readList1(int i, float f, std::string s)
{
    return { i, f, s };
}

SERIALIZETABLE_BEGIN(TestUnit)
SYNC(action)
SYNC(query)

FIELD(list1)
FIELD(list2)
FIELD(set1)
FIELD(set2)
FIELD(map1, PairCreator<DefaultCreator<const std::pair<int, float>>, DefaultCreator<int>>)

FIELD(complexList1, CustomCreator<&readList1, &writeList1>)
FIELD(complexList2)
FIELD(complexList3)

FIELD(pod)

SERIALIZETABLE_END(TestUnit)

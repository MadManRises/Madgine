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

void writeList1(Engine::Serialize::FormattedSerializeStream &out, const ComplexDataType &data)
{
    out.beginExtendedWrite("Item", 4);
    write(out, data.i, "first");
    write(out, data.f, "second");
    write(out, data.s, "third");
    write(out, data.b, "fourth");
}
Engine::Serialize::StreamResult readList1(Engine::Serialize::FormattedSerializeStream &in, int &i, float &f, std::string &s, bool &b)
{
    STREAM_PROPAGATE_ERROR(in.beginExtendedRead("Item", 4));
    STREAM_PROPAGATE_ERROR(read(in, i, "first"));
    STREAM_PROPAGATE_ERROR(read(in, f, "second"));
    STREAM_PROPAGATE_ERROR(read(in, s, "third"));
    STREAM_PROPAGATE_ERROR(read(in, b, "fourth"));
    return {};
}

SERIALIZETABLE_BEGIN(TestUnit)
SYNC(action)
SYNC(query)

FIELD(list1)
FIELD(list2)
FIELD(set1)
FIELD(set2)
FIELD(map1, KeyValueCreator<DefaultCreator, DefaultCreator>)

FIELD(complexList1, CustomCreator<&readList1, &writeList1>)
FIELD(complexList2)
FIELD(complexList3)

FIELD(pod)

FIELD(bytes)

SERIALIZETABLE_END(TestUnit)

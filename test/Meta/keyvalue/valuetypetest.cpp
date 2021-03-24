#include <gtest/gtest.h>

#include "Meta/metalib.h"

#include "Meta/keyvalue/functiontable_impl.h"
#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/keyvalue/valuetype.h"

#include "Meta/keyvalue/lambda.h"

#include "Meta/keyvalue/keyvaluepair.h"

int testF(int i)
{
    return 3 + i;
}

FUNCTIONTABLE(testF, i)

struct ValueType_Foo {
    int operator()(int i) { return testF(i); }
};

METATABLE_BEGIN(ValueType_Foo)
CALL_OPERATOR(i)
METATABLE_END(ValueType_Foo)

struct ValueType_Bar {
    int operator()() { return 4; }
};

METATABLE_BEGIN(ValueType_Bar)
CALL_OPERATOR()
METATABLE_END(ValueType_Bar)

TEST(ValueType, Construct)
{
    using namespace Engine;
    using namespace std::literals::string_view_literals;

    ValueType_Foo foo;

    ValueType test;
    test = "abc"sv;
    test.clear();
    test = true;
    test = 3;
    test = (uint64_t)4;
    test = 5.0f;
    test = Vector2 { 6.1f, 6.2f };
    test = Vector3 { 7.1f, 7.2f, 7.3f };


    test = &function<&testF>();
    ASSERT_EQ(test(1).as<int>(), 4);
    test = &testF;
    ASSERT_EQ(test(2).as<int>(), 5);
    test = foo;
    ASSERT_EQ(test(3).as<int>(), 6);
    test = ValueType_Foo {};
    ASSERT_EQ(test(4).as<int>(), 7);
    int dummy = 5;
    test = lambda([=](int i) { return testF(i + dummy); });
    ASSERT_EQ(test(5).as<int>(), 13);

    test = lambda(ValueType_Bar{});
    ASSERT_EQ(test().as<int>(), 4);
    test = ValueType_Bar {};
    ASSERT_EQ(test().as<int>(), 4);

    
    std::vector<int> v = { 1, 2, 3 };
    test = v;
    std::next(test.as<KeyValueVirtualRange>().begin())->mValue = ValueType { 4 };
}

#include <gtest/gtest.h>

#include "Modules/moduleslib.h"

TEST(TupleUnpacker, ToTuple)
{
    struct S {
        int i;
        bool b;
    };

    S s { 5, false };

    auto t = Engine::TupleUnpacker::toTuple(s);

	static_assert(std::is_same_v<std::tuple<int &, bool&>, decltype(t)>);

    ASSERT_EQ(std::get<0>(t), 5);
    ASSERT_EQ(std::get<1>(t), false);

    s.b = true;

    ASSERT_EQ(std::get<1>(t), true);

    std::get<0>(t) = 7;

    ASSERT_EQ(s.i, 7);

    auto t2 = Engine::TupleUnpacker::toTuple(S { 11, true });

	static_assert(std::is_same_v<std::tuple<int, bool>, decltype(t2)>);

    ASSERT_EQ(std::get<0>(t2), 11);
    ASSERT_EQ(std::get<1>(t2), true);
}

#include <gtest/gtest.h>

#include "Modules/moduleslib.h"

#include "Modules/keyvalue/typedscopeptr.h"
#include "Modules/keyvalue/metatable_impl.h"

struct Foo {
    int i;
};

METATABLE_BEGIN(Foo)
MEMBER(i)
METATABLE_END(Foo)

struct Bar : Foo {
    float f;
};

METATABLE_BEGIN_BASE(Bar, Foo)
MEMBER(f)
METATABLE_END(Bar)

TEST(KeyValue, TypedScopePtr)
{
    Bar b;
    b.i = 1;
    b.f = 1.6f;
    Engine::TypedScopePtr ptr = &b;
    Foo *f = ptr.safe_cast<Foo>();
    ASSERT_EQ(f, &b);
}

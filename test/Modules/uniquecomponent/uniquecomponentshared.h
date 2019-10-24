#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#if defined(TestShared_EXPORTS)
#    define TEST_EXPORT DLL_EXPORT
#else
#    define TEST_EXPORT DLL_IMPORT
#endif

struct TestDriver {
};

struct TestBase {
    virtual ~TestBase() = default;
};

DECLARE_UNIQUE_COMPONENT(Test, Test, TestBase, TestDriver &);

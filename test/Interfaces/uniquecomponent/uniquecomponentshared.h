#pragma once

#include "Interfaces/uniquecomponent/uniquecomponentdefine.h"

#if defined(STATIC_BUILD)
#	define TEST_EXPORT
#	define TEST_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION
#else
#	if defined(TestShared_EXPORTS)
#		define TEST_EXPORT DLL_EXPORT
#		define TEST_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION_EXPORT
#	else
#		define TEST_EXPORT DLL_IMPORT
#		define TEST_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION_IMPORT
#	endif
#endif

struct TestDriver 
{

};

struct TestBase 
{
	virtual ~TestBase() = default;
};

RegisterType(TestBase);

DEFINE_UNIQUE_COMPONENT(Test, TestBase, TestDriver&, Test, TEST);
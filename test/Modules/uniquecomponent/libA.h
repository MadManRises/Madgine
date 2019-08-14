#pragma once

#if defined(STATIC_BUILD)
#	define LIBA_EXPORT
#else
#	if defined(LibA_EXPORTS)
#		define LIBA_EXPORT DLL_EXPORT
#		define LIBA_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION_EXPORT
#	else
#		define LIBA_EXPORT DLL_IMPORT
#		define LIBA_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION_IMPORT
#	endif
#endif

#include "uniquecomponentshared.h"

struct LIBA_EXPORT LibAComponent : Test::TestComponent<LibAComponent>
{

	LibAComponent(TestDriver &driver)
	{

	}

};
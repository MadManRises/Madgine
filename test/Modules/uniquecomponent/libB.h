#pragma once

#if defined(STATIC_BUILD)
#	define LIBB_EXPORT
#else
#	if defined(LibB_EXPORTS)
#		define LIBB_EXPORT DLL_EXPORT
#	else
#		define LIBB_EXPORT DLL_IMPORT
#	endif
#endif

#include "uniquecomponentshared.h"

struct LIBB_EXPORT LibBComponent : Test::TestComponent<LibBComponent>
{

	LibBComponent(TestDriver &driver)
	{

	}

};
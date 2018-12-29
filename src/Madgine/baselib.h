#pragma once

/// @cond

#include "Interfaces/interfaceslib.h"

#if defined(STATIC_BUILD)
#	define MADGINE_BASE_EXPORT
#	define MADGINE_BASE_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION
#else
#	if defined(Base_EXPORTS)
#		define MADGINE_BASE_EXPORT DLL_EXPORT
#		define MADGINE_BASE_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION_EXPORT
#	else
#		define MADGINE_BASE_EXPORT DLL_IMPORT
#		define MADGINE_BASE_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION_IMPORT
#	endif
#endif


#include "baseforward.h"

#include <set>
#include <queue>




/// @endcond

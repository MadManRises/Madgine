#pragma once


#if defined(STATIC_BUILD)
#	define MADGINE_SDLINPUT_EXPORT
#	define MADGINE_SDLINPUT_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION
#else
#	if defined(SDLInput_EXPORTS)
#		define MADGINE_SDLINPUT_EXPORT DLL_EXPORT
#		define MADGINE_SDLINPUT_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION_EXPORT
#	else
#		define MADGINE_SDLINPUT_EXPORT DLL_IMPORT
#		define MADGINE_SDLINPUT_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION_IMPORT
#	endif
#endif


#include "clientlib.h"
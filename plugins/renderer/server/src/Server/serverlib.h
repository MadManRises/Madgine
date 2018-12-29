#pragma once

/// @cond

#if defined(STATIC_BUILD)
#	define MADGINE_SERVER_EXPORT
#	define MADGINE_SERVER_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION
#else
#	if defined(Server_EXPORTS)
#		define MADGINE_SERVER_EXPORT DLL_EXPORT
#		define MADGINE_SERVER_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION_EXPORT
#	else
#		define MADGINE_SERVER_EXPORT DLL_IMPORT
#		define MADGINE_SERVER_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION_IMPORT
#	endif
#endif

#include "serverforward.h"

#include "Madgine/baselib.h"

#include <thread>
#include <fstream>

/// @endcond

#pragma once

/// @cond

#if defined(STATIC_BUILD)
#define MADGINE_OIS_EXPORT
#else
#if defined(OISHandler_EXPORTS)
#define MADGINE_OIS_EXPORT DLL_EXPORT
#else
#define MADGINE_OIS_EXPORT DLL_IMPORT
#endif
#endif

#include "Madgine/clientlib.h"


/// @endcond

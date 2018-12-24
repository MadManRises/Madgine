#pragma once

#include "Madgine/clientlib.h"

#include "toolsforward.h"


#if defined(STATIC_BUILD)
#define MADGINE_TOOLS_EXPORT
#else
#if defined(Tools_EXPORTS)
#define MADGINE_TOOLS_EXPORT DLL_EXPORT
#else
#define MADGINE_TOOLS_EXPORT DLL_IMPORT
#endif
#endif

#include <algorithm>
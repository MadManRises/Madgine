#pragma once

#include "Madgine/baselib.h"
#include "Madgine/resourceslib.h"

#include "toolsforward.h"

#if defined(Tools_EXPORTS)
#    define MADGINE_TOOLS_EXPORT DLL_EXPORT
#else
#    define MADGINE_TOOLS_EXPORT DLL_IMPORT
#endif

#include <algorithm>
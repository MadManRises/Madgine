#pragma once

#include "Madgine/rootlib.h"

#if defined(Tools_EXPORTS)
#    define MADGINE_TOOLS_EXPORT DLL_EXPORT
#else
#    define MADGINE_TOOLS_EXPORT DLL_IMPORT
#endif

#include "toolsforward.h"

#include <algorithm>
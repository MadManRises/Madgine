#pragma once

#include "Madgine_Tools/toolslib.h"
#include "OpenAL/openallib.h"

#if defined(OpenALPluginTools_EXPORTS)
#    define MADGINE_OPENAL_TOOLS_EXPORT DLL_EXPORT
#else
#    define MADGINE_OPENAL_TOOLS_EXPORT DLL_IMPORT
#endif

#include "openaltoolsforward.h"
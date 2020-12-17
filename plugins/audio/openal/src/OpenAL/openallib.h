#pragma once

/// @cond

#if defined(OpenALPlugin_EXPORTS)
#    define MADGINE_OPENAL_EXPORT DLL_EXPORT
#else
#    define MADGINE_OPENAL_EXPORT DLL_IMPORT
#endif

#include "openalforward.h"

#include "Madgine/scenelib.h"
#include "audioloaderlib.h"

/// @endcond

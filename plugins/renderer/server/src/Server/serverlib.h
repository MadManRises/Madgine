#pragma once

/// @cond

#if defined(STATIC_BUILD)
#    define MADGINE_SERVERRENDERER_EXPORT
#else
#    if defined(ServerRenderer_EXPORTS)
#        define MADGINE_SERVERRENDERER_EXPORT DLL_EXPORT
#    else
#        define MADGINE_SERVERRENDERER_EXPORT DLL_IMPORT
#    endif
#endif

#include "serverforward.h"

#include "Madgine/baselib.h"

#include <fstream>
#include <thread>

/// @endcond

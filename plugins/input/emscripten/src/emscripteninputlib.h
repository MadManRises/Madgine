#pragma once

#include "Madgine/clientlib.h"

#if defined(STATIC_BUILD)
#    define MADGINE_EMSCRIPTENINPUT_EXPORT
#else
#    if defined(EmscriptenInput_EXPORTS)
#        define MADGINE_EMSCRIPTENINPUT_EXPORT DLL_EXPORT
#    else
#        define MADGINE_EMSCRIPTENINPUT_EXPORT DLL_IMPORT
#    endif
#endif

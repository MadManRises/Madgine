#pragma once

#if defined(STATIC_BUILD)
#    define MADGINE_SDLINPUT_EXPORT
#else
#    if defined(SDLInput_EXPORTS)
#        define MADGINE_SDLINPUT_EXPORT DLL_EXPORT
#    else
#        define MADGINE_SDLINPUT_EXPORT DLL_IMPORT
#    endif
#endif

#include "clientlib.h"
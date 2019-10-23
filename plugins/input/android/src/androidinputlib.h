#pragma once

#include "Madgine/clientlib.h"

#if defined(STATIC_BUILD)
#    define MADGINE_ANDROIDINPUT_EXPORT
#else
#    if defined(AndroidInput_EXPORTS)
#        define MADGINE_ANDROIDINPUT_EXPORT DLL_EXPORT
#    else
#        define MADGINE_ANDROIDINPUT_EXPORT DLL_IMPORT
#    endif
#endif

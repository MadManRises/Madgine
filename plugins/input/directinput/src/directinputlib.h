#pragma once

#include "Madgine/clientlib.h"

#if defined(DirectInput_EXPORTS)
#    define MADGINE_DIRECTINPUT_EXPORT DLL_EXPORT
#else
#    define MADGINE_DIRECTINPUT_EXPORT DLL_IMPORT
#endif

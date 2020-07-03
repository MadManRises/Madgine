#pragma once

#include "Modules/moduleslib.h"

#if defined(Ini_EXPORTS)
#    define MADGINE_INI_EXPORT DLL_EXPORT
#else
#    define MADGINE_INI_EXPORT DLL_IMPORT
#endif
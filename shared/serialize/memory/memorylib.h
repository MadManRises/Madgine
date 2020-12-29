#pragma once

#include "Modules/moduleslib.h"

#if defined(Memory_EXPORTS)
#    define MADGINE_MEMORY_EXPORT DLL_EXPORT
#else
#    define MADGINE_MEMORY_EXPORT DLL_IMPORT
#endif
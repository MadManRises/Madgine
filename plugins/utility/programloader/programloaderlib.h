#pragma once

#include "Modules/moduleslib.h"

#if defined(ProgramLoader_EXPORTS)
#    define MADGINE_PROGRAMLOADER_EXPORT DLL_EXPORT
#else
#    define MADGINE_PROGRAMLOADER_EXPORT DLL_IMPORT
#endif

#include "programloaderforward.h"

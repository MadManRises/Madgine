#pragma once

#include "Modules/moduleslib.h"
#include "Madgine/resourceslib.h"
#include "codegen/codegenlib.h"

#if defined(ProgramLoader_EXPORTS)
#    define MADGINE_PROGRAMLOADER_EXPORT DLL_EXPORT
#else
#    define MADGINE_PROGRAMLOADER_EXPORT DLL_IMPORT
#endif

#include "programloaderforward.h"

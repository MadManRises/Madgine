#pragma once

#include "Modules/moduleslib.h"
#include "Madgine/resourceslib.h"

#if defined(MeshLoader_EXPORTS)
#    define MADGINE_MESHLOADER_EXPORT DLL_EXPORT
#else
#    define MADGINE_MESHLOADER_EXPORT DLL_IMPORT
#endif

#include "meshloaderforward.h"

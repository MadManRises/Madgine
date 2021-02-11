#pragma once

#include "Modules/moduleslib.h"
#include "Madgine/resourceslib.h"

#if defined(SkeletonLoader_EXPORTS)
#    define MADGINE_SKELETONLOADER_EXPORT DLL_EXPORT
#else
#    define MADGINE_SKELETONLOADER_EXPORT DLL_IMPORT
#endif

#include "skeletonloaderforward.h"

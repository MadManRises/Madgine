#pragma once

#include "Madgine/resourceslib.h"
#include "Madgine/imageloaderlib.h"

#if defined(TextureLoader_EXPORTS)
#    define MADGINE_TEXTURELOADER_EXPORT DLL_EXPORT
#else
#    define MADGINE_TEXTURELOADER_EXPORT DLL_IMPORT
#endif

#include "textureloaderforward.h"

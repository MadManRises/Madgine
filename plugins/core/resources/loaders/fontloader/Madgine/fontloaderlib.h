#pragma once

#include "Madgine/resourceslib.h"
#include "Madgine/textureloaderlib.h"
#include "Madgine/serialize/memory/memorylib.h"
#include "Madgine/serialize/filesystem/filesystemlib.h"

#if defined(FontLoader_EXPORTS)
#    define MADGINE_FONTLOADER_EXPORT DLL_EXPORT
#else
#    define MADGINE_FONTLOADER_EXPORT DLL_IMPORT
#endif

#include "fontloaderforward.h"

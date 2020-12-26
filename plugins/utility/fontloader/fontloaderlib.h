#pragma once

#if defined(FontLoader_EXPORTS)
#    define MADGINE_FONTLOADER_EXPORT DLL_EXPORT
#else
#    define MADGINE_FONTLOADER_EXPORT DLL_IMPORT
#endif

#include "Modules/moduleslib.h"
#include "textureloaderlib.h"

#include "fontloaderforward.h"

#pragma once

#include "Modules/moduleslib.h"
#include "textureloaderlib.h"


#if defined(FontLoader_EXPORTS)
#    define MADGINE_FONTLOADER_EXPORT DLL_EXPORT
#else
#    define MADGINE_FONTLOADER_EXPORT DLL_IMPORT
#endif

#include "fontloaderforward.h"

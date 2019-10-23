#pragma once

#include "Modules/moduleslib.h"

#if defined(STATIC_BUILD)
#    define MADGINE_IMAGELOADER_EXPORT
#else
#    if defined(ImageLoader_EXPORTS)
#        define MADGINE_IMAGELOADER_EXPORT DLL_EXPORT
#    else
#        define MADGINE_IMAGELOADER_EXPORT DLL_IMPORT
#    endif
#endif
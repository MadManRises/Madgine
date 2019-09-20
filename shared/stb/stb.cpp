
#include "Modules/moduleslib.h"

#if defined(STATIC_BUILD)
#    define STBIDEF
#else
#    if defined(stb_EXPORTS)
#        define STBIDEF DLL_EXPORT
#    else
#        define STBIDEF DLL_IMPORT
#    endif
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
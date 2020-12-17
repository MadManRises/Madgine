#pragma once

#include "Modules/moduleslib.h"

#if defined(AudioLoader_EXPORTS)
#    define MADGINE_AUDIOLOADER_EXPORT DLL_EXPORT
#else
#    define MADGINE_AUDIOLOADER_EXPORT DLL_IMPORT
#endif

#include "audioloaderforward.h"

#pragma once

#include "Meta/metalib.h"
#include "Interfaces/interfaceslib.h"

#if defined(Filesystem_EXPORTS)
#    define MADGINE_FILESYSTEM_EXPORT DLL_EXPORT
#else
#    define MADGINE_FILESYSTEM_EXPORT DLL_IMPORT
#endif
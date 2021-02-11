#pragma once

#include "Interfaces/interfaceslib.h"


#if defined(Ini_EXPORTS)
#    define INI_EXPORT DLL_EXPORT
#else
#    define INI_EXPORT DLL_IMPORT
#endif

#include "iniforward.h"
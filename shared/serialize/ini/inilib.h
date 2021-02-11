#pragma once

#include "Meta/metalib.h"

#if defined(IniSerialize_EXPORTS)
#    define MADGINE_INI_EXPORT DLL_EXPORT
#else
#    define MADGINE_INI_EXPORT DLL_IMPORT
#endif
#pragma once

#include "Meta/metalib.h"
#include "Modules/moduleslib.h"

#if defined(Network_EXPORTS)
#    define MADGINE_NETWORK_EXPORT DLL_EXPORT
#else
#    define MADGINE_NETWORK_EXPORT DLL_IMPORT
#endif
#pragma once

/// @cond

#if defined(Client_EXPORTS)
#    define MADGINE_CLIENT_EXPORT DLL_EXPORT
#else
#    define MADGINE_CLIENT_EXPORT DLL_IMPORT
#endif

#include "Modules/moduleslib.h"
#include "Meta/metalib.h"

#include "programloaderlib.h"
#include "meshloaderlib.h"

#include "clientforward.h"

#include "render/renderforward.h"


/// @endcond

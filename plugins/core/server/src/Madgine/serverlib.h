#pragma once

/// @cond

#include "Madgine/baselib.h"

#if defined(Server_EXPORTS)
#    define MADGINE_SERVER_EXPORT DLL_EXPORT
#else
#    define MADGINE_SERVER_EXPORT DLL_IMPORT
#endif

#include "serverforward.h"

#include <queue>
#include <set>

/// @endcond

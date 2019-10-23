#pragma once

/// @cond

#include "Madgine/baselib.h"

#if defined(STATIC_BUILD)
#    define MADGINE_SERVER_EXPORT
#else
#    if defined(Server_EXPORTS)
#        define MADGINE_SERVER_EXPORT DLL_EXPORT
#    else
#        define MADGINE_SERVER_EXPORT DLL_IMPORT
#    endif
#endif

#include "serverforward.h"

#include <queue>
#include <set>

/// @endcond

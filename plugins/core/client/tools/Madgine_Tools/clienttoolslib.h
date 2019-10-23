#pragma once

/// @cond

#if defined(STATIC_BUILD)
#    define MADGINE_CLIENT_TOOLS_EXPORT
#else
#    if defined(ClientTools_EXPORTS)
#        define MADGINE_CLIENT_TOOLS_EXPORT DLL_EXPORT
#    else
#        define MADGINE_CLIENT_TOOLS_EXPORT DLL_IMPORT
#    endif
#endif

#include "Madgine/clientlib.h"
#include "toolslib.h"

#include "clienttoolsforward.h"
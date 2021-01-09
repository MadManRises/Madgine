#pragma once

#if defined(ClientTools_EXPORTS)
#    define MADGINE_CLIENT_TOOLS_EXPORT DLL_EXPORT
#else
#    define MADGINE_CLIENT_TOOLS_EXPORT DLL_IMPORT
#endif

#include "toolslib.h"
#include "Madgine/clientlib.h"
#include "programloaderlib.h"
#include "textureloaderlib.h"

#include "clienttoolsforward.h"
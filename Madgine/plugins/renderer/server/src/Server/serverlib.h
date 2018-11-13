#pragma once

/// @cond

#if defined(STATIC_BUILD)
#define MADGINE_SERVER_EXPORT
#else
#if defined(Server_EXPORTS)
#define MADGINE_SERVER_EXPORT DLL_EXPORT
#else
#define MADGINE_SERVER_EXPORT DLL_IMPORT
#endif
#endif

#include "serverforward.h"

#include "Madgine/baselib.h"

#include <thread>
#include <fstream>

/// @endcond

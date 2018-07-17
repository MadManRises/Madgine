#pragma once

/// @cond

#if defined(MadgineServer_EXPORTS)
#define MADGINE_SERVER_EXPORT DLL_EXPORT
#else
#define MADGINE_SERVER_EXPORT DLL_IMPORT
#endif

#include "serverforward.h"

#include "Madgine/baselib.h"

#include <thread>
#include <fstream>
#include <chrono>

/// @endcond

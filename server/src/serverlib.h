#pragma once

/// @cond

#if defined(MadgineServer_EXPORTS)
#ifdef __GNUC__
#define MADGINE_SERVER_EXPORT //__attribute__((dllexport))
#else
#define MADGINE_SERVER_EXPORT __declspec(dllexport)
#endif
#else
#ifdef __GNUC__
#define MADGINE_SERVER_EXPORT //__attribute__((dllexport))
#else
#define MADGINE_SERVER_EXPORT __declspec(dllimport)
#endif
#endif

#include "serverforward.h"

#include "baselib.h"

#include <thread>
#include <fstream>
#include <chrono>

/// @endcond
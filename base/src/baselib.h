#pragma once

/// @cond

#if defined(Base_EXPORTS)
#ifdef __GNUC__
#define MADGINE_BASE_EXPORT //__attribute__((dllexport))
#else
#define MADGINE_BASE_EXPORT __declspec(dllexport)
#endif
#else
#ifdef __GNUC__
#define MADGINE_BASE_EXPORT //__attribute__((dllimport))
#else
#define MADGINE_BASE_EXPORT __declspec(dllimport)
#endif
#endif

#include "baseforward.h"

#include <set>
#include <queue>

#include "interfaceslib.h"

/// @endcond

#pragma once

/// @cond

#ifdef _WIN32
#ifdef __GNUC__
#define DLL_EXPORT __attribute__((dllexport))
#else
#define DLL_EXPORT __declspec(dllexport)
#endif
#else
#define DLL_EXPORT __attribute__((visibility("default")))
#endif

#ifdef _WIN32
#ifdef __GNUC__
#define DLL_IMPORT __attribute__((dllimport))
#else
#define DLL_IMPORT __declspec(dllimport)
#endif
#else
#define DLL_IMPORT
#endif

#if defined(Maditor_Shared_EXPORTS)
#define MADITOR_SHARED_EXPORT DLL_EXPORT
#else
#define MADITOR_SHARED_EXPORT DLL_IMPORT
#endif



#include "maditorsharedforward.h"


#include <Madgine/interfaceslib.h>






/// @endcond
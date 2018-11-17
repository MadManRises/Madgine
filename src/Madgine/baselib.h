#pragma once

/// @cond

#include "Interfaces/interfaceslib.h"

#if defined(STATIC_BUILD)
#define MADGINE_BASE_EXPORT
#else
#if defined(Base_EXPORTS)
#define MADGINE_BASE_EXPORT DLL_EXPORT
#else
#define MADGINE_BASE_EXPORT DLL_IMPORT
#endif
#endif


#include "baseforward.h"

#include <set>
#include <queue>




/// @endcond

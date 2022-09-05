#pragma once

/// @cond

#include "Madgine/codegen/codegenlib.h"
#include "Madgine/cli/clilib.h"
#include "Modules/moduleslib.h"

#if defined(Root_EXPORTS)
#    define MADGINE_ROOT_EXPORT DLL_EXPORT
#else
#    define MADGINE_ROOT_EXPORT DLL_IMPORT
#endif

#include "rootforward.h"

#include <queue>
#include <set>

/// @endcond

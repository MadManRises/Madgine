#pragma once

#include "Madgine/codegen/codegenlib.h"

/// @cond

#include "Modules/moduleslib.h"
#include "Madgine/resourceslib.h"

#if defined(NodeGraph_EXPORTS)
#    define MADGINE_NODEGRAPH_EXPORT DLL_EXPORT
#else
#    define MADGINE_NODEGRAPH_EXPORT DLL_IMPORT
#endif

#include "nodegraphforward.h"

/// @endcond

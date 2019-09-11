#pragma once

/// @cond

#if defined(STATIC_BUILD)
#    define MADGINE_CLIENT_TOOLS_EXPORT
#    define MADGINE_CLIENT_TOOLS_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION
#else
#    if defined(ClientTools_EXPORTS)
#        define MADGINE_CLIENT_TOOLS_EXPORT DLL_EXPORT
#        define MADGINE_CLIENT_TOOLS_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION_EXPORT
#    else
#        define MADGINE_CLIENT_TOOLS_EXPORT DLL_IMPORT
#        define MADGINE_CLIENT_TOOLS_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION_IMPORT
#    endif
#endif

#include "Madgine/clientlib.h"
#include "toolslib.h"

#include "clienttoolsforward.h"
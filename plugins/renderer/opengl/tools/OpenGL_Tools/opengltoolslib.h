#pragma once

/// @cond

#if defined(STATIC_BUILD)
#    define MADGINE_OPENGL_TOOLS_EXPORT
#    define MADGINE_OPENGL_TOOLS_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION
#else
#    if defined(OpenGLTools_EXPORTS)
#        define MADGINE_OPENGL_TOOLS_EXPORT DLL_EXPORT
#        define MADGINE_OPENGL_TOOLS_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION_EXPORT
#    else
#        define MADGINE_OPENGL_TOOLS_EXPORT DLL_IMPORT
#        define MADGINE_OPENGL_TOOLS_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION_IMPORT
#    endif
#endif

#include "OpenGL/opengllib.h"
#include "Madgine_Tools/clienttoolslib.h"

#include "opengltoolsforward.h"
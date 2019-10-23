#pragma once

/// @cond

#if defined(STATIC_BUILD)
#    define MADGINE_OPENGL_TOOLS_EXPORT
#else
#    if defined(OpenGLTools_EXPORTS)
#        define MADGINE_OPENGL_TOOLS_EXPORT DLL_EXPORT
#    else
#        define MADGINE_OPENGL_TOOLS_EXPORT DLL_IMPORT
#    endif
#endif

#include "OpenGL/opengllib.h"
#include "Madgine_Tools/clienttoolslib.h"

#include "opengltoolsforward.h"
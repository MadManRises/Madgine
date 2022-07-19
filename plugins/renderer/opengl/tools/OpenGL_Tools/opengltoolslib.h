#pragma once

#if defined(OpenGLTools_EXPORTS)
#    define MADGINE_OPENGL_TOOLS_EXPORT DLL_EXPORT
#else
#    define MADGINE_OPENGL_TOOLS_EXPORT DLL_IMPORT
#endif

#include "Madgine_Tools/clienttoolslib.h"
#include "OpenGL/opengllib.h"

#include "opengltoolsforward.h"
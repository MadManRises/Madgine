#pragma once

/// @cond

#if defined(STATIC_BUILD)
#    define MADGINE_OPENGL_EXPORT
#    define MADGINE_OPENGL_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION
#else
#    if defined(OpenGL_EXPORTS)
#        define MADGINE_OPENGL_EXPORT DLL_EXPORT
#        define MADGINE_OPENGL_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION_EXPORT
#    else
#        define MADGINE_OPENGL_EXPORT DLL_IMPORT
#        define MADGINE_OPENGL_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION_IMPORT
#    endif
#endif

#include "openglforward.h"

#include "Madgine/clientlib.h"

#if !ANDROID && !EMSCRIPTEN
#    include "../glad/glad.h"
#    define OPENGL_ES 0
#else
#    include <GLES3/gl3.h>
#    define OPENGL_ES 1
#endif

MADGINE_OPENGL_EXPORT void glDump();

inline void glCheck()
{
    int e = glGetError();
    if (e) {
        LOG("GL-Error: " << e);
        glDump();
        std::terminate();
        throw e;
    }
}

#define GL_CHECK() glCheck()
//#define GL_LOG(x) LOG("GL: " << x)
#define GL_LOG(x)

#if WINDOWS

#    undef NO_ERROR
typedef HGLRC ContextHandle;

#elif LINUX

struct __GLXcontextRec;
typedef struct __GLXcontextRec *GLXContext;

typedef GLXContext ContextHandle;

#elif ANDROID || EMSCRIPTEN

typedef void *EGLContext;
typedef EGLContext ContextHandle;

#else

#    error "Unsupported Platform!"

#endif

/// @endcond

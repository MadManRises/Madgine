#pragma once

/// @cond

#if defined(OpenGL_EXPORTS)
#    define MADGINE_OPENGL_EXPORT DLL_EXPORT
#else
#    define MADGINE_OPENGL_EXPORT DLL_IMPORT
#endif

#include "openglforward.h"

#include "Madgine/clientlib.h"
#include "meshloaderlib.h"
#include "programloaderlib.h"
#include "fontloaderlib.h"

#if !ANDROID && !EMSCRIPTEN
#    include "../glad/glad.h"
#    define OPENGL_ES 0
#else
#    include <GLES3/gl32.h>
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
    }
}

#if !EMSCRIPTEN
#    define GL_CHECK() glCheck()
#else
#    define GL_CHECK()
#endif
//#define GL_LOG(x) LOG("GL: " << x)
#define GL_LOG(x)

#if WINDOWS

#define NOMINMAX
#include <Windows.h>

#    undef NO_ERROR
typedef HGLRC ContextHandle;

#elif LINUX

struct __GLXcontextRec;
typedef struct __GLXcontextRec *GLXContext;

typedef GLXContext ContextHandle;

#elif ANDROID || EMSCRIPTEN

typedef void *EGLContext;
typedef EGLContext ContextHandle;

#elif OSX


typedef void *ContextHandle;

#else

#    error "Unsupported Platform!"

#endif

/// @endcond

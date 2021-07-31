#pragma once

/// @cond

#if defined(OpenGL_EXPORTS)
#    define MADGINE_OPENGL_EXPORT DLL_EXPORT
#else
#    define MADGINE_OPENGL_EXPORT DLL_IMPORT
#endif

#include "openglforward.h"

#include "meshloaderlib.h"
#include "programloaderlib.h"
#include "textureloaderlib.h"
#include "Madgine/clientlib.h"

#if !ANDROID && !EMSCRIPTEN && !IOS
#    include "../glad/glad.h"
#    define OPENGL_ES 1
#elif IOS
#    include <OpenGLES/ES3/gl.h>
#    define OPENGL_ES 1
#else
#    include <GLES3/gl32.h>
#    define OPENGL_ES 1
#endif

#if EMSCRIPTEN
#    define WEBGL 1
#else
#    define WEBGL 0
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

#    define NOMINMAX
#    include <Windows.h>

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

#elif IOS

typedef void *ContextHandle;

#else

#    error "Unsupported Platform!"

#endif

/// @endcond

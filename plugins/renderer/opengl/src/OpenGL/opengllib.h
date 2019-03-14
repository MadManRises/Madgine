#pragma once

/// @cond

#if defined(STATIC_BUILD)
#	define MADGINE_OPENGL_EXPORT
#	define MADGINE_OPENGL_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION
#else
#	if defined(OpenGL_EXPORTS)
#		define MADGINE_OPENGL_EXPORT DLL_EXPORT
#		define MADGINE_OPENGL_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION_EXPORT
#	else
#		define MADGINE_OPENGL_EXPORT DLL_IMPORT
#		define MADGINE_OPENGL_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION_IMPORT
#	endif
#endif

#include "openglforward.h"

#include "client/clientlib.h"

#if !ANDROID
#include "../glad/glad.h"
#else
#include <GLES3/gl3.h>
#endif

inline void glCheck() { int e = glGetError(); if (e) throw e; };

#if WINDOWS

typedef HGLRC ContextHandle;

#elif LINUX

struct __GLXcontextRec;
typedef struct __GLXcontextRec *GLXContext;

typedef GLXContext ContextHandle;

#elif ANDROID

typedef void *EGLContext;
typedef EGLContext ContextHandle;

#endif



/// @endcond

#pragma once

/// @cond

#if defined(STATIC_BUILD)
#define MADGINE_OPENGL_EXPORT
#else
#if defined(OpenGL_EXPORTS)
#define MADGINE_OPENGL_EXPORT DLL_EXPORT
#else
#define MADGINE_OPENGL_EXPORT DLL_IMPORT
#endif
#endif

#include "openglforward.h"

#include "Madgine/clientlib.h"

#define NOMINMAX
#include <Windows.h>

typedef HGLRC(WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);
typedef BOOL(WINAPI * PFNWGLSWAPINTERVALEXTPROC) (int interval);

/// @endcond

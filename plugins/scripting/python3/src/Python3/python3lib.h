#pragma once

/// @cond

#include "Madgine/baselib.h"
#include "Madgine/resourceslib.h"

#if defined(Python3_EXPORTS)
#    define MADGINE_PYTHON3_EXPORT DLL_EXPORT
#else
#    define MADGINE_PYTHON3_EXPORT DLL_IMPORT
#endif

#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include "python3forward.h"


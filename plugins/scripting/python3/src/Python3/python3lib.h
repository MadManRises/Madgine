#pragma once

/// @cond

#if defined(Python3_EXPORTS)
#    define MADGINE_PYTHON3_EXPORT DLL_EXPORT
#else
#    define MADGINE_PYTHON3_EXPORT DLL_IMPORT
#endif

#define Py_DEBUG
#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include "python3forward.h"

#include "Madgine/baselib.h"

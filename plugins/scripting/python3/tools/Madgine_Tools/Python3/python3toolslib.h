#pragma once

#if defined(Python3Tools_EXPORTS)
#    define MADGINE_PYTHON3_TOOLS_EXPORT DLL_EXPORT
#else
#    define MADGINE_PYTHON3_TOOLS_EXPORT DLL_IMPORT
#endif

#include "toolslib.h"
#include "Python3/python3lib.h"

#include "python3toolsforward.h"
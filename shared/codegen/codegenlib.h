#pragma once

#include "Generic/genericlib.h"

#include <variant>
#include <sstream>
#include <stack>

#if defined(CodeGen_EXPORTS)
#    define MADGINE_CODEGEN_EXPORT DLL_EXPORT
#else
#    define MADGINE_CODEGEN_EXPORT DLL_IMPORT
#endif
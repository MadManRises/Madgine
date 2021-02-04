#pragma once

#include "Interfaces/interfaceslib.h"


#if defined(CodeGen_EXPORTS)
#    define MADGINE_CODEGEN_EXPORT DLL_EXPORT
#else
#    define MADGINE_CODEGEN_EXPORT DLL_IMPORT
#endif
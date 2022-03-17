#pragma once

#include "Interfaces/interfaceslib.h"

#include "modulesconfig.h"

/// @cond

#include "cli/clilib.h"
#include "ini/inilib.h"

#if defined(Modules_EXPORTS)
#    define MODULES_EXPORT DLL_EXPORT
#else
#    define MODULES_EXPORT DLL_IMPORT
#endif

#include <shared_mutex>
#include <regex>
#include <variant>
#include <stack>


#include "modulesforward.h"




/// @endcond

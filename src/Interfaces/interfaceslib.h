#pragma once

/// @cond

#include "Generic/genericlib.h"

#include "interfacesconfig.h"



#if defined(Interfaces_EXPORTS)
#    define INTERFACES_EXPORT DLL_EXPORT
#else
#    define INTERFACES_EXPORT DLL_IMPORT
#endif



#include "debug/memory/memoryglobal.h"


#include <array>
#include <fstream>
#include <unordered_map>
#include <optional>
#include <cstring>


#include "interfacesforward.h"
#include "util/utilmethods.h"

/// @endcond

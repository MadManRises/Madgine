#pragma once

/// @cond

#if defined(Client_EXPORTS)
#    define MADGINE_CLIENT_EXPORT DLL_EXPORT
#else
#    define MADGINE_CLIENT_EXPORT DLL_IMPORT
#endif

#include "Modules/moduleslib.h"
#include "programloaderlib.h"
#include "imageloaderlib.h"
#include "meshloaderlib.h"
#include "textureloaderlib.h"
#include "fontloaderlib.h"

#include <array>
#include <future>
#include <map>
#include <memory>
#include <sstream>
#include <stack>
#include <typeindex>
#include <vector>

#include "clientforward.h"


/// @endcond

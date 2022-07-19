#pragma once

#include "Madgine/server/serverlib.h"
#include "Madgine/serialize/network/networklib.h"


#if defined(MMOLobby_EXPORTS)
#    define MMOLOBBY_EXPORT DLL_EXPORT
#else
#    define MMOLOBBY_EXPORT DLL_IMPORT
#endif

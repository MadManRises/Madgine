#pragma once

#include "Madgine/serverlib.h"
#include "serialize/network/networklib.h"


#if defined(MMOLobby_EXPORTS)
#    define MMOLOBBY_EXPORT DLL_EXPORT
#else
#    define MMOLOBBY_EXPORT DLL_IMPORT
#endif

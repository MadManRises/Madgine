#pragma once

#include "Madgine/controlslib.h"
#include "Madgine/scenelib.h"
#include "Madgine/bulletlib.h"


#if defined(PlayerControls_EXPORTS)
#    define MADGINE_PLAYERCONTROLS_EXPORT DLL_EXPORT
#else
#    define MADGINE_PLAYERCONTROLS_EXPORT DLL_IMPORT
#endif

#include "playercontrolsforward.h"
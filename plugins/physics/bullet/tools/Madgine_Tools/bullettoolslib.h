#pragma once

#include "Madgine_Tools/clienttoolslib.h"
#include "Madgine/bulletlib.h"

#if defined(BulletTools_EXPORTS)
#    define MADGINE_BULLET_TOOLS_EXPORT DLL_EXPORT
#else
#    define MADGINE_BULLET_TOOLS_EXPORT DLL_IMPORT
#endif

#include "bullettoolsforward.h"
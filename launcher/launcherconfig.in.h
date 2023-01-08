#pragma once

#cmakedefine MADGINE_LAUNCHER_WINDOW_TITLE "${MADGINE_LAUNCHER_WINDOW_TITLE}"

#if WINDOWS
#    include "resources.h"
#else
#    define MADGINE_LAUNCHER_ICON 0
#endif
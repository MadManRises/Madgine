#if defined(MADGINE_SERVER_BUILD) && defined(MADGINE_CLIENT_BUILD)
#error "Both Server- and Client-config have been selected!"
#elif defined(MADGINE_SERVER_BUILD)
#include "serverlib.h"
#elif defined(MADGINE_CLIENT_BUILD)
#include "ogrelib.h"
#else
#error "No config has been selected!"
#endif
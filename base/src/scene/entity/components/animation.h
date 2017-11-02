#pragma once


#if defined(MADGINE_SERVER_BUILD)
#include "serveranimation.h"
#elif defined(MADGINE_CLIENT_BUILD)
#include "ogreanimation.h"
#endif
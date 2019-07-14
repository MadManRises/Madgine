#pragma once

#include "Interfaces/macros.h"

#if !defined(STATIC_BUILD)
#	if defined(PROJECT_NAME)
#		define PLUGIN_LOCAL(name) CONCAT2(CONCAT(name, _), PROJECT_NAME)
#	else
#		define PLUGIN_LOCAL(name) __UNDEFINED__##name
#	endif
#endif
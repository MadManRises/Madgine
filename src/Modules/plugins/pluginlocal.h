#pragma once

#include "Interfaces/macros.h"

#if ENABLE_PLUGINS
#	if !defined(PROJECT_NAME)
#		warning "When building Plugins, PROJECT_NAME should be defined for all projects"
#	endif
#	define PLUGIN_LOCAL(name) CONCAT2(CONCAT(name, _), PROJECT_NAME)
#endif
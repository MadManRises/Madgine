#pragma once

#include "Interfaces/macros.h"

#if ENABLE_PLUGINS
#    if !defined(PROJECT_NAME)
#        ifdef _MSC_VER
#            pragma message ": warning<>: When building Plugins, PROJECT_NAME should be defined for all projects"
#        else
#            warning "When building Plugins, PROJECT_NAME should be defined for all projects"
#        endif
#    endif
#    define PLUGIN_LOCAL(name) CONCAT2(CONCAT(name, _), PROJECT_NAME)
#endif
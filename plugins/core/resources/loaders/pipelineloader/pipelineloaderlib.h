#pragma once

#include "Madgine/resourceslib.h"
#include "Modules/moduleslib.h"
#include "Madgine/codegen/codegenlib.h"

#if defined(PipelineLoader_EXPORTS)
#    define MADGINE_PIPELINELOADER_EXPORT DLL_EXPORT
#else
#    define MADGINE_PIPELINELOADER_EXPORT DLL_IMPORT
#endif

#include "pipelineloaderforward.h"


#pragma once

#if defined(DirectX12Tools_EXPORTS)
#    define MADGINE_DIRECTX12_TOOLS_EXPORT DLL_EXPORT
#else
#    define MADGINE_DIRECTX12_TOOLS_EXPORT DLL_IMPORT
#endif

#include "Madgine_Tools/clienttoolslib.h"
#include "DirectX12/directx12lib.h"

#include "directx12toolsforward.h"
#pragma once

#include "Madgine_Tools/clienttoolslib.h"
#include "DirectX11/directx11lib.h"

#if defined(DirectX11Tools_EXPORTS)
#    define MADGINE_DIRECTX11_TOOLS_EXPORT DLL_EXPORT
#else
#    define MADGINE_DIRECTX11_TOOLS_EXPORT DLL_IMPORT
#endif

#include "directx11toolsforward.h"
#pragma once

/// @cond

#include "Madgine/meshloaderlib.h"
#include "Madgine/pipelineloaderlib.h"
#include "Madgine/textureloaderlib.h"
#include "Madgine/clientlib.h"

#if defined(DirectX11_EXPORTS)
#    define MADGINE_DIRECTX11_EXPORT DLL_EXPORT
#else
#    define MADGINE_DIRECTX11_EXPORT DLL_IMPORT
#endif

#include "directx11forward.h"

#define NOMINMAX

#include <d3d11.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <comdef.h>
#undef ERROR

MADGINE_DIRECTX11_EXPORT void dx11Dump();

inline void dx11Check(HRESULT result)
{
    if (FAILED(result)) {
        _com_error error { result };
        LOG_FATAL("DX11-Error: " << error.ErrorMessage());
        dx11Dump();
        std::terminate();
    }
}

#define DX11_CHECK(result) dx11Check(result)

#define DX11_LOG(x) LOG_DEBUG("DX11: " << x)

#include <span>

#include "Interfaces/helpers/win_ptrs.h"
#include "Interfaces/helpers/win_wstring.h"

/// @endcond

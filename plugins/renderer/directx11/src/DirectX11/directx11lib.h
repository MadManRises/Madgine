#pragma once

/// @cond

#if defined(DirectX11_EXPORTS)
#    define MADGINE_DIRECTX11_EXPORT DLL_EXPORT
#else
#    define MADGINE_DIRECTX11_EXPORT DLL_IMPORT
#endif

#include "directx11forward.h"

#include "Madgine/clientlib.h"
#include "meshloaderlib.h"
#include "programloaderlib.h"
#include "textureloaderlib.h"

#define NOMINMAX

#include <d3d11.h>
#include <d3dcompiler.h>

MADGINE_DIRECTX11_EXPORT void dx11Dump();

inline void dx11Check(HRESULT result)
{
    if (FAILED(result)) {
        LOG("DX11-Error: " << "?");
        dx11Dump();
        std::terminate();
    }
}

#define DX11_CHECK(result) dx11Check(result)

//#define DX11_LOG(x) LOG("DX11: " << x)
#define DX11_LOG(x)

/// @endcond

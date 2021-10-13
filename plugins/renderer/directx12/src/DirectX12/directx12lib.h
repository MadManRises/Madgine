#pragma once

/// @cond

#if defined(DirectX12_EXPORTS)
#    define MADGINE_DIRECTX12_EXPORT DLL_EXPORT
#else
#    define MADGINE_DIRECTX12_EXPORT DLL_IMPORT
#endif

#include "directx12forward.h"

#include "meshloaderlib.h"
#include "programloaderlib.h"
#include "textureloaderlib.h"
#include "Madgine/clientlib.h"

#define NOMINMAX

#include "d3dx12.h"
#include <d3d12.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <pix.h>

MADGINE_DIRECTX12_EXPORT void dx12Dump(HRESULT result);
MADGINE_DIRECTX12_EXPORT bool checkDevice(HRESULT &result);

inline void dx12Check(HRESULT result = 0)
{
    if (FAILED(result) || !checkDevice(result)) {
        LOG("DX12-Error: "
            << "?");
        dx12Dump(result);
        std::terminate();
    }
}

#define DX12_CHECK(...) dx12Check(__VA_ARGS__)

//#define DX12_LOG(x) LOG("DX12: " << x)
#define DX12_LOG(x)

#include "Generic/offsetptr.h"

constexpr D3D12_CPU_DESCRIPTOR_HANDLE operator+(D3D12_CPU_DESCRIPTOR_HANDLE handle, Engine::OffsetPtr offset)
{
    return { reinterpret_cast<uintptr_t>(reinterpret_cast<std::byte*>(handle.ptr) + offset) };
}

constexpr D3D12_GPU_DESCRIPTOR_HANDLE operator+(D3D12_GPU_DESCRIPTOR_HANDLE handle, Engine::OffsetPtr offset)
{
    return { reinterpret_cast<uintptr_t>(reinterpret_cast<std::byte *>(handle.ptr) + offset) };
}

constexpr Engine::OffsetPtr operator-(D3D12_CPU_DESCRIPTOR_HANDLE handle1, D3D12_CPU_DESCRIPTOR_HANDLE handle2)
{
    return Engine::OffsetPtr { handle1.ptr - handle2.ptr };
}

constexpr Engine::OffsetPtr operator-(D3D12_GPU_DESCRIPTOR_HANDLE handle1, D3D12_GPU_DESCRIPTOR_HANDLE handle2)
{
    return Engine::OffsetPtr { handle1.ptr - handle2.ptr };
}

/// @endcond

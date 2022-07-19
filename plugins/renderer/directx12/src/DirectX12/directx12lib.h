#pragma once

/// @cond

#if defined(DirectX12_EXPORTS)
#    define MADGINE_DIRECTX12_EXPORT DLL_EXPORT
#else
#    define MADGINE_DIRECTX12_EXPORT DLL_IMPORT
#endif

#include "directx12forward.h"

#include "meshloaderlib.h"
#include "pipelineloaderlib.h"
#include "textureloaderlib.h"
#include "Madgine/clientlib.h"

#define NOMINMAX

#include <d3d12.h>
#include <dxgi1_6.h>
#include <pix.h>
#include "d3dx12.h"
#include <dxcapi.h>

MADGINE_DIRECTX12_EXPORT void dx12Dump(HRESULT result);
MADGINE_DIRECTX12_EXPORT bool checkDevice(HRESULT &result);

#define MADGINE_DIRECTX12_USE_SINGLE_COMMAND_LIST 0

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

struct ReleaseDeleter {
    template <typename T>
    void operator()(T *ptr)
    {
        ptr->Release();
    }
};
template <typename T>
struct ReleasePtr : std::unique_ptr<T, ReleaseDeleter> {
    using std::unique_ptr<T, ReleaseDeleter>::unique_ptr;

    T **operator&()
    {
        assert(!*this);
        return reinterpret_cast<T **>(this);
    }

    T *const *operator&() const
    {
        return reinterpret_cast<T *const *>(this);
    }

    operator T *() const
    {
        return this->get();
    }
};

constexpr D3D12_CPU_DESCRIPTOR_HANDLE operator+(D3D12_CPU_DESCRIPTOR_HANDLE handle, Engine::OffsetPtr offset)
{
    return { handle.ptr + offset.offset() };
}

constexpr D3D12_GPU_DESCRIPTOR_HANDLE operator+(D3D12_GPU_DESCRIPTOR_HANDLE handle, Engine::OffsetPtr offset)
{
    return { handle.ptr + offset.offset() };
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

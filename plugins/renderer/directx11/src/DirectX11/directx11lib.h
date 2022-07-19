#pragma once

/// @cond

#if defined(DirectX11_EXPORTS)
#    define MADGINE_DIRECTX11_EXPORT DLL_EXPORT
#else
#    define MADGINE_DIRECTX11_EXPORT DLL_IMPORT
#endif

#include "directx11forward.h"

#include "meshloaderlib.h"
#include "pipelineloaderlib.h"
#include "textureloaderlib.h"
#include "Madgine/clientlib.h"

#include "Modules/threading/workgroup.h"

#define NOMINMAX

#include <d3d11.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>

MADGINE_DIRECTX11_EXPORT void dx11Dump();

inline void dx11Check(HRESULT result)
{
    if (FAILED(result)) {
        LOG("DX11-Error: "
            << "?");
        dx11Dump();
        std::terminate();
    }
}

#define DX11_CHECK(result) dx11Check(result)

#define DX11_LOG(x) LOG_DEBUG("DX11: " << x << " (" << Engine::Threading::WorkGroup::self().name() << ")")

#include <span>

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

/// @endcond

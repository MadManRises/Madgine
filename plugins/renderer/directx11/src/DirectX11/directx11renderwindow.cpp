#include "directx11lib.h"

#include "directx11renderwindow.h"

#include "Modules/debug/profiler/profile.h"

#include "Meta/math/vector2i.h"
#include "Meta/math/vector3.h"
#include "Meta/math/vector3i.h"
#include "Meta/math/vector4.h"

#include "directx11rendertexture.h"

#include "Interfaces/window/windowapi.h"

#include "directx11meshloader.h"
#include "directx11pixelshaderloader.h"
#include "directx11programloader.h"
#include "directx11rendercontext.h"
#include "directx11textureloader.h"
#include "directx11vertexshaderloader.h"

#include "Meta/keyvalue/metatable_impl.h"

namespace Engine {
namespace Render {

    DirectX11RenderWindow::DirectX11RenderWindow(DirectX11RenderContext *context, Window::OSWindow *w, DirectX11RenderWindow *reusedResources)
        : DirectX11RenderTarget(context)
        , mWindow(w)
        , mReusedContext(reusedResources)
    {
        HRESULT hr;

        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

        swapChainDesc.BufferCount = 1;
        swapChainDesc.BufferDesc.Width = w->renderSize().x;
        swapChainDesc.BufferDesc.Height = w->renderSize().y;
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferDesc.RefreshRate = /*QueryRefreshRate(clientWidth, clientHeight, vSync)*/ { 60, 1 };
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = reinterpret_cast<HWND>(w->mHandle);
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        swapChainDesc.Windowed = TRUE;

        if (!reusedResources) {
            UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;

            // These are the feature levels that we will accept.
            D3D_FEATURE_LEVEL featureLevels[] = {
                D3D_FEATURE_LEVEL_11_1,
                D3D_FEATURE_LEVEL_11_0,
                D3D_FEATURE_LEVEL_10_1,
                D3D_FEATURE_LEVEL_10_0,
                D3D_FEATURE_LEVEL_9_3,
                D3D_FEATURE_LEVEL_9_2,
                D3D_FEATURE_LEVEL_9_1
            };

            // This will be the feature level that
            // is used to create our device and swap chain.
            D3D_FEATURE_LEVEL featureLevel;

            assert(sDeviceContext == nullptr);

            hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
                nullptr, createDeviceFlags, featureLevels, _countof(featureLevels),
                D3D11_SDK_VERSION, &swapChainDesc, &mSwapChain, &sDevice, &featureLevel,
                &sDeviceContext);

            if (hr == E_INVALIDARG) {
                hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
                    nullptr, createDeviceFlags, &featureLevels[1], _countof(featureLevels) - 1,
                    D3D11_SDK_VERSION, &swapChainDesc, &mSwapChain, &sDevice, &featureLevel,
                    &sDeviceContext);
            }

            DX11_CHECK(hr);
        } else {
            IDXGIDevice *device;
            hr = sDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&device);
            DX11_CHECK(hr);

            IDXGIAdapter *adapter;
            hr = device->GetParent(__uuidof(IDXGIAdapter), (void **)&adapter);
            DX11_CHECK(hr);

            IDXGIFactory *factory;
            hr = adapter->GetParent(__uuidof(IDXGIFactory), (void **)&factory);
            DX11_CHECK(hr);

            hr = factory->CreateSwapChain(sDevice, &swapChainDesc, &mSwapChain);
            DX11_CHECK(hr);
        }

        // The Direct3D device and swap chain were successfully created.
        // Now we need to initialize the buffers of the swap chain.
        // Next initialize the back buffer of the swap chain and associate it to a
        // render target view.
        ID3D11Texture2D *backBuffer;
        hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&backBuffer);
        DX11_CHECK(hr);

        ID3D11RenderTargetView *targetView;

        hr = sDevice->CreateRenderTargetView(backBuffer, nullptr, &targetView);
        DX11_CHECK(hr);

        if (backBuffer) {
            backBuffer->Release();
            backBuffer = nullptr;
        }

        InterfacesVector size = w->renderSize();
        setup(targetView, { size.x, size.y });
    }

    DirectX11RenderWindow::~DirectX11RenderWindow()
    {
        shutdown();

        if (mSwapChain) {
            mSwapChain->Release();
            mSwapChain = nullptr;
        }

        if (!mReusedContext) {
            if (sDeviceContext) {
                sDeviceContext->Release();
                sDeviceContext = nullptr;
            }
            if (sDevice) {
                sDevice->Release();
                sDevice = nullptr;
            }
        }
    }

    void DirectX11RenderWindow::beginFrame()
    {
        PROFILE();

        DirectX11RenderTarget::beginFrame();
    }

    void DirectX11RenderWindow::endFrame()
    {
        DirectX11RenderTarget::endFrame();

        mSwapChain->Present(0, 0);
    }

    Texture *DirectX11RenderWindow::texture() const
    {
        return nullptr;
    }

    Vector2i DirectX11RenderWindow::size() const
    {
        InterfacesVector size = mWindow->renderSize();
        return { size.x, size.y };
    }

    bool DirectX11RenderWindow::resize(const Vector2i &size)
    {
        if (mTargetView) {
            mTargetView->Release();
            mTargetView = nullptr;
        }

        HRESULT hr = mSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
        DX11_CHECK(hr);

        ID3D11Texture2D *backBuffer;
        hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&backBuffer);
        DX11_CHECK(hr);

        ID3D11RenderTargetView *targetView;

        hr = sDevice->CreateRenderTargetView(backBuffer, nullptr, &targetView);
        DX11_CHECK(hr);

        if (backBuffer) {
            backBuffer->Release();
            backBuffer = nullptr;
        }

        setup(targetView, size);

        return true;
    }
}
}
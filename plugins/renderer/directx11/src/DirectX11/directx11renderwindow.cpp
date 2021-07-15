#include "directx11lib.h"

#include "directx11renderwindow.h"

#include "Modules/debug/profiler/profile.h"

#include "Meta/math/vector2i.h"
#include "Meta/math/vector3.h"
#include "Meta/math/vector3i.h"
#include "Meta/math/vector4.h"

#include "directx11rendertexture.h"

#include "Interfaces/window/windowapi.h"


#include "Meta/keyvalue/metatable_impl.h"

#include "directx11rendercontext.h"

namespace Engine {
namespace Render {

    DirectX11RenderWindow::DirectX11RenderWindow(DirectX11RenderContext *context, Window::OSWindow *w)
        : DirectX11RenderTarget(context)
        , mWindow(w)
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

        IDXGIDevice *device;
        hr = sDevice->QueryInterface(IID_PPV_ARGS(& device));
        DX11_CHECK(hr);

        IDXGIAdapter *adapter;
        hr = device->GetParent(IID_PPV_ARGS(&adapter));
        DX11_CHECK(hr);

        IDXGIFactory *factory;
        hr = adapter->GetParent(IID_PPV_ARGS(&factory));
        DX11_CHECK(hr);

        hr = factory->CreateSwapChain(sDevice, &swapChainDesc, &mSwapChain);
        DX11_CHECK(hr);

        // The Direct3D device and swap chain were successfully created.
        // Now we need to initialize the buffers of the swap chain.
        // Next initialize the back buffer of the swap chain and associate it to a
        // render target view.
        ID3D11Texture2D *backBuffer;
        hr = mSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
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

    TextureHandle DirectX11RenderWindow::depthTexture() const
    {
        return 0;
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
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
        : DirectX11RenderTarget(context, true, w->title())
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

        ReleasePtr<IDXGIDevice> device;
        hr = sDevice->QueryInterface(IID_PPV_ARGS(&device));
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
        ReleasePtr<ID3D11Texture2D> backBuffer;
        hr = mSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
        DX11_CHECK(hr);

        ReleasePtr<ID3D11RenderTargetView> targetView;

        hr = sDevice->CreateRenderTargetView(backBuffer, nullptr, &targetView);
        DX11_CHECK(hr);

        InterfacesVector size = w->renderSize();

        std::vector<ReleasePtr<ID3D11RenderTargetView>> views;
        views.emplace_back(std::move(targetView));
        setup(std::move(views), { size.x, size.y }, TextureType_2D);
    }

    DirectX11RenderWindow::~DirectX11RenderWindow()
    {
        shutdown();
    }

    void DirectX11RenderWindow::beginIteration(size_t iteration) const
    {
        PROFILE();

        DirectX11RenderTarget::beginIteration(iteration);
    }

    void DirectX11RenderWindow::endIteration(size_t iteration) const
    {
        DirectX11RenderTarget::endIteration(iteration);

        mSwapChain->Present(0, 0);
    }

    TextureDescriptor DirectX11RenderWindow::texture(size_t index, size_t iteration) const
    {
        return {};
    }

    size_t DirectX11RenderWindow::textureCount() const
    {
        return 0;
    }

    TextureDescriptor DirectX11RenderWindow::depthTexture() const
    {
        return {};
    }

    Vector2i DirectX11RenderWindow::size() const
    {
        InterfacesVector size = mWindow->renderSize();
        return { size.x, size.y };
    }

    bool DirectX11RenderWindow::resizeImpl(const Vector2i &size)
    {
        mTargetViews.clear();

        HRESULT hr = mSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
        DX11_CHECK(hr);

        ReleasePtr<ID3D11Texture2D> backBuffer;
        hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&backBuffer);
        DX11_CHECK(hr);

        ReleasePtr<ID3D11RenderTargetView> targetView;

        hr = sDevice->CreateRenderTargetView(backBuffer, nullptr, &targetView);
        DX11_CHECK(hr);

        std::vector<ReleasePtr<ID3D11RenderTargetView>> views;
        views.emplace_back(std::move(targetView));
        setup(std::move(views), size, TextureType_2D);

        return true;
    }
}
}
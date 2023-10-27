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

    DirectX11RenderWindow::DirectX11RenderWindow(DirectX11RenderContext *context, Window::OSWindow *w, size_t samples)
        : DirectX11RenderTarget(context, true, w->title())
        , mWindow(w)
    {
        HRESULT hr;

        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

        swapChainDesc.BufferCount = 2;
        swapChainDesc.BufferDesc.Width = w->renderSize().x;
        swapChainDesc.BufferDesc.Height = w->renderSize().y;
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferDesc.RefreshRate = /*QueryRefreshRate(clientWidth, clientHeight, vSync)*/ { 60, 1 };
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = reinterpret_cast<HWND>(w->mHandle);
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
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

        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

        hr = sDevice->CreateRenderTargetView(backBuffer, &rtvDesc, &targetView);
        DX11_CHECK(hr);

        InterfacesVector size = w->renderSize();

        std::vector<std::array<ReleasePtr<ID3D11RenderTargetView>, 6>> views;
        views.emplace_back()[0] = std::move(targetView);
        setup(std::move(views), { size.x, size.y }, TextureType_2D, samples);
    }

    DirectX11RenderWindow::~DirectX11RenderWindow()
    {
        shutdown();
    }

    void DirectX11RenderWindow::beginIteration(bool flipFlopping, size_t targetIndex, size_t targetCount, size_t targetSubresourceIndex) const
    {
        PROFILE();

        DirectX11RenderTarget::beginIteration(flipFlopping, targetIndex, targetCount, targetSubresourceIndex);
    }

    void DirectX11RenderWindow::endIteration() const
    {
        DirectX11RenderTarget::endIteration();
    }

    void DirectX11RenderWindow::endFrame()
    {
        DirectX11RenderTarget::endFrame();

        mSwapChain->Present(0, 0);
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

        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

        hr = sDevice->CreateRenderTargetView(backBuffer, &rtvDesc, &targetView);
        DX11_CHECK(hr);

        std::vector<std::array<ReleasePtr<ID3D11RenderTargetView>, 6>> views;
        views.emplace_back()[0] = std::move(targetView);
        setup(std::move(views), size, TextureType_2D);

        return true;
    }
}
}
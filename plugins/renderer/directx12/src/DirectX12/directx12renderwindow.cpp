#include "directx12lib.h"

#include "directx12renderwindow.h"

#include "Modules/debug/profiler/profile.h"

#include "Meta/math/vector2i.h"
#include "Meta/math/vector3.h"
#include "Meta/math/vector3i.h"
#include "Meta/math/vector4.h"

#include "directx12rendertexture.h"

#include "Interfaces/window/windowapi.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "directx12rendercontext.h"

namespace Engine {
namespace Render {

    DirectX12RenderWindow::DirectX12RenderWindow(DirectX12RenderContext *context, Window::OSWindow *w)
        : DirectX12RenderTarget(context)
        , mWindow(w)        
    {
        //context->waitForGPU();

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

        IDXGISwapChain *swapChain;
        hr = context->mFactory->CreateSwapChain(context->mCommandQueue, &swapChainDesc, &swapChain);
        DX12_CHECK(hr);

        hr = swapChain->QueryInterface(&mSwapChain);        
        DX12_CHECK(hr);

        mTargetViews[0] = context->mRenderTargetDescriptorHeap.allocate();
        mTargetViews[1] = context->mRenderTargetDescriptorHeap.allocate();

        createRenderTargetViews();

        // The Direct3D device and swap chain were successfully created.
        // Now we need to initialize the buffers of the swap chain.
        // Next initialize the back buffer of the swap chain and associate it to a
        // render target view.

        InterfacesVector size = w->renderSize();
        setup(mTargetViews[0], { size.x, size.y });
    }

    DirectX12RenderWindow::~DirectX12RenderWindow()
    {
        shutdown();

        if (mSwapChain) {
            mSwapChain->Release();
            mSwapChain = nullptr;
        }
    }

    void DirectX12RenderWindow::beginFrame()
    {
        PROFILE();

        mTargetView = mTargetViews[1 - mSwapChain->GetCurrentBackBufferIndex()];

        DirectX12RenderTarget::beginFrame();
    }

    void DirectX12RenderWindow::endFrame()
    {
        DirectX12RenderTarget::endFrame();

        mSwapChain->Present(0, 0);
    }

    Texture *DirectX12RenderWindow::texture() const
    {
        return nullptr;
    }

    TextureHandle DirectX12RenderWindow::depthTexture() const
    {
        return 0;
    }

    Vector2i DirectX12RenderWindow::size() const
    {
        InterfacesVector size = mWindow->renderSize();
        return { size.x, size.y };
    }

    void DirectX12RenderWindow::createRenderTargetViews()
    {
        for (size_t i = 0; i < 2; ++i) {
            ID3D12Resource *backBuffer;
            HRESULT hr = mSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer));
            DX12_CHECK(hr);

            backBuffer->SetName((L"Window - BackBuffer " + std::to_wstring(i)).c_str());

            assert(backBuffer);

            sDevice->CreateRenderTargetView(backBuffer, nullptr, DirectX12RenderContext::getSingleton().mRenderTargetDescriptorHeap.cpuHandle(mTargetViews[i]));
            
            backBuffer->Release();
        }
    }

    bool DirectX12RenderWindow::resize(const Vector2i &size)
    {
        HRESULT hr = mSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
        DX12_CHECK(hr);

        createRenderTargetViews();

        setup(mTargetViews[0], size);

        return true;
    }
}
}
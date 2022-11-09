#include "directx12lib.h"

#include "directx12renderwindow.h"

#include "Modules/debug/profiler/profile.h"

#include "Meta/math/vector2i.h"

#include "Interfaces/window/windowapi.h"

#include "directx12rendercontext.h"

namespace Engine {
namespace Render {

    DirectX12RenderWindow::DirectX12RenderWindow(DirectX12RenderContext *context, Window::OSWindow *w, size_t samples)
        : DirectX12RenderTarget(context, true, w->title())
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
        swapChainDesc.SampleDesc.Count = samples;
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
    }

    void DirectX12RenderWindow::beginIteration(size_t iteration) const
    {
        PROFILE();

        mTargetView = mTargetViews[mSwapChain->GetCurrentBackBufferIndex()];

        Transition(mBackBuffers[mSwapChain->GetCurrentBackBufferIndex()], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

        DirectX12RenderTarget::beginIteration(iteration);
    }

    void DirectX12RenderWindow::endIteration(size_t iteration) const
    {
        Transition(mBackBuffers[mSwapChain->GetCurrentBackBufferIndex()], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

        DirectX12RenderTarget::endIteration(iteration);

        mSwapChain->Present(0, 0);
    }

    Vector2i DirectX12RenderWindow::size() const
    {
        InterfacesVector size = mWindow->renderSize();
        return { size.x, size.y };
    }

    void DirectX12RenderWindow::createRenderTargetViews()
    {
        for (size_t i = 0; i < 2; ++i) {
            HRESULT hr = mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mBackBuffers[i]));
            DX12_CHECK(hr);

            assert(mBackBuffers[i]);

            mBackBuffers[i]->SetName((L"Window - BackBuffer " + std::to_wstring(i)).c_str());

            GetDevice()->CreateRenderTargetView(mBackBuffers[i], nullptr, DirectX12RenderContext::getSingleton().mRenderTargetDescriptorHeap.cpuHandle(mTargetViews[i]));
        }
    }

    bool DirectX12RenderWindow::resizeImpl(const Vector2i &size)
    {
        HRESULT hr = mSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
        DX12_CHECK(hr);

        createRenderTargetViews();

        setup(mTargetViews[0], size);

        return true;
    }
}
}
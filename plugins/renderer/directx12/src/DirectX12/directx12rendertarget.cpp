#include "directx12lib.h"

#include "directx12rendertarget.h"

#include "Madgine/meshloader/meshdata.h"

#include "directx12meshdata.h"
#include "directx12meshloader.h"
#include "util/directx12pipelineinstance.h"

#include "directx12rendercontext.h"
#include "directx12rendertexture.h"

#include "Meta/math/rect2i.h"

#include "Madgine/render/renderpass.h"

namespace Engine {
namespace Render {

    DirectX12RenderTarget::DirectX12RenderTarget(DirectX12RenderContext *context, bool global, std::string name, RenderTarget *blitSource)
        : RenderTarget(context, global, name, 1, blitSource)
    {
        mDepthStencilView = DirectX12RenderContext::getSingleton().mDepthStencilDescriptorHeap.allocate();
    }

    DirectX12RenderTarget::~DirectX12RenderTarget()
    {
    }

    void DirectX12RenderTarget::setup(std::vector<OffsetPtr> targetViews, const Vector2i &size, size_t samples, bool createDepthBufferView)
    {
        mTargetViews = targetViews;

        mDepthStencilBuffer.reset();

        D3D12_HEAP_PROPERTIES depthHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

        D3D12_RESOURCE_DESC depthResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R24G8_TYPELESS, size.x, size.y, 1, 1, samples);
        depthResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        CD3DX12_CLEAR_VALUE depthOptimizedClearValue(DXGI_FORMAT_D24_UNORM_S8_UINT, 1.0f, 0);

        HRESULT hr = GetDevice()->CreateCommittedResource(
            &depthHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &depthResourceDesc,
            createDepthBufferView ? D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE : D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthOptimizedClearValue,
            IID_PPV_ARGS(&mDepthStencilBuffer));
        DX12_CHECK(hr);

        mDepthStencilBuffer->SetName(StringUtil::toWString(name() + "DepthTexture").c_str());

        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc {};
        dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
        dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsvDesc.ViewDimension = samples > 1 ? D3D12_DSV_DIMENSION_TEXTURE2DMS : D3D12_DSV_DIMENSION_TEXTURE2D;

        GetDevice()->CreateDepthStencilView(mDepthStencilBuffer, &dsvDesc, DirectX12RenderContext::getSingleton().mDepthStencilDescriptorHeap.cpuHandle(mDepthStencilView));

        mSamples = samples;
    }

    void DirectX12RenderTarget::shutdown()
    {
        mDepthStencilBuffer.reset();
    }

    void DirectX12RenderTarget::beginFrame()
    {
        mCommandList = context()->fetchCommandList(name());

        mCommandList->SetGraphicsRootSignature(context()->mRootSignature);

        mCommandList.attachResource(mDepthStencilBuffer);

        for (RenderPass *pass : renderPasses()) {
            for (RenderData *data : pass->dependencies()) {
                DirectX12RenderTexture *tex = dynamic_cast<DirectX12RenderTexture *>(data);
                if (tex) {
                    for (const DirectX12Texture &texture : tex->textures())
                        mCommandList.attachResource(texture);
                }
            }
        }

        RenderTarget::beginFrame();
    }

    void DirectX12RenderTarget::endFrame()
    {
        RenderTarget::endFrame();

        mCommandList.reset();
    }

    void DirectX12RenderTarget::beginIteration(size_t iteration) const
    {

        const Vector2i &screenSize = size();

        D3D12_VIEWPORT viewport;
        viewport.Width = static_cast<float>(screenSize.x);
        viewport.Height = static_cast<float>(screenSize.y);
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        mCommandList->RSSetViewports(1, &viewport);

        D3D12_RECT scissorRect;
        scissorRect.left = 0.0f;
        scissorRect.top = 0.0f;
        scissorRect.right = static_cast<float>(screenSize.x);
        scissorRect.bottom = static_cast<float>(screenSize.y);

        mCommandList->RSSetScissorRects(1, &scissorRect);

        //TransitionBarrier();

        constexpr FLOAT color[4] = { 0.033f, 0.073f, 0.073f, 1.0f };

        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> targetViews { mTargetViews.size() };
        for (size_t i = 0; i < targetViews.size(); ++i)
            targetViews[i] = DirectX12RenderContext::getSingleton().mRenderTargetDescriptorHeap.cpuHandle(mTargetViews[i]);
        D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = DirectX12RenderContext::getSingleton().mDepthStencilDescriptorHeap.cpuHandle(mDepthStencilView);

        if (!mBlitSource) {
            for (D3D12_CPU_DESCRIPTOR_HANDLE &targetView : targetViews)
                mCommandList->ClearRenderTargetView(targetView, color, 0, nullptr);
            mCommandList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
        }

        mCommandList->OMSetRenderTargets(targetViews.size(), targetViews.data(), false, &depthStencilView);
        /*sDeviceContext->OMSetDepthStencilState(mDepthStencilState, 1);
        sDeviceContext->OMSetBlendState(mBlendState, 0, 0xffffffff);*/

        //sDeviceContext->PSSetSamplers(0, 2, mSamplers);

        RenderTarget::beginIteration(iteration);
    }

    void DirectX12RenderTarget::endIteration(size_t iteration) const
    {
        RenderTarget::endIteration(iteration);
    }

    void DirectX12RenderTarget::setRenderSpace(const Rect2i &space)
    {
        D3D12_VIEWPORT viewport;
        viewport.Width = static_cast<float>(space.mSize.x);
        viewport.Height = static_cast<float>(space.mSize.y);
        viewport.TopLeftX = static_cast<float>(space.mTopLeft.x);
        viewport.TopLeftY = static_cast<float>(space.mTopLeft.y);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        mCommandList->RSSetViewports(1, &viewport);

        D3D12_RECT scissorRect;
        scissorRect.left = static_cast<float>(space.mTopLeft.x);
        scissorRect.top = static_cast<float>(space.mTopLeft.y);
        scissorRect.right = static_cast<float>(space.mTopLeft.x + space.mSize.x);
        scissorRect.bottom = static_cast<float>(space.mTopLeft.y + space.mSize.y);

        mCommandList->RSSetScissorRects(1, &scissorRect);
    }

    void DirectX12RenderTarget::pushAnnotation(const char *tag)
    {
        PIXBeginEvent(mCommandList, PIX_COLOR(255, 255, 255), tag);
    }

    void DirectX12RenderTarget::popAnnotation()
    {
        PIXEndEvent(mCommandList);
    }

    void DirectX12RenderTarget::clearDepthBuffer()
    {
        mCommandList->ClearDepthStencilView(DirectX12RenderContext::getSingleton().mDepthStencilDescriptorHeap.cpuHandle(mDepthStencilView), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    }

    DirectX12RenderContext *DirectX12RenderTarget::context() const
    {
        return static_cast<DirectX12RenderContext *>(RenderTarget::context());
    }

    size_t DirectX12RenderTarget::samples() const
    {
        return mSamples;
    }

}
}

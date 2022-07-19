#include "directx12lib.h"

#include "directx12rendertarget.h"

#include "meshdata.h"

#include "directx12meshdata.h"
#include "directx12meshloader.h"
#include "util/directx12pipelineinstance.h"

#include "directx12rendercontext.h"

#include "Meta/math/rect2i.h"

#include "render/material.h"

namespace Engine {
namespace Render {

    DirectX12RenderTarget::DirectX12RenderTarget(DirectX12RenderContext *context, bool global, std::string name)
        : RenderTarget(context, global, name)
    {
    }

    DirectX12RenderTarget::~DirectX12RenderTarget()
    {
    }

    void DirectX12RenderTarget::setup(OffsetPtr targetView, const Vector2i &size)
    {
        mTargetView = targetView;

        mDepthStencilBuffer.reset();

        D3D12_HEAP_PROPERTIES depthHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

        D3D12_RESOURCE_DESC depthResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R24G8_TYPELESS, size.x, size.y);
        depthResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        CD3DX12_CLEAR_VALUE depthOptimizedClearValue(DXGI_FORMAT_D24_UNORM_S8_UINT, 1.0f, 0);

        HRESULT hr = GetDevice()->CreateCommittedResource(
            &depthHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &depthResourceDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthOptimizedClearValue,
            IID_PPV_ARGS(&mDepthStencilBuffer));
        DX12_CHECK(hr);

        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc {};
        dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
        dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

        if (!mDepthStencilView)
            mDepthStencilView = DirectX12RenderContext::getSingleton().mDepthStencilDescriptorHeap.allocate();
        GetDevice()->CreateDepthStencilView(mDepthStencilBuffer, &dsvDesc, DirectX12RenderContext::getSingleton().mDepthStencilDescriptorHeap.cpuHandle(mDepthStencilView));
    }

    void DirectX12RenderTarget::shutdown()
    {
        mDepthStencilBuffer.reset();
    }

    void DirectX12RenderTarget::beginIteration(size_t iteration) const
    {
        ID3D12GraphicsCommandList *commandList = context()->mCommandList.mList;

        commandList->SetGraphicsRootSignature(context()->mRootSignature);

        const Vector2i &screenSize = size();

        D3D12_VIEWPORT viewport;
        viewport.Width = static_cast<float>(screenSize.x);
        viewport.Height = static_cast<float>(screenSize.y);
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        commandList->RSSetViewports(1, &viewport);

        D3D12_RECT scissorRect;
        scissorRect.left = 0.0f;
        scissorRect.top = 0.0f;
        scissorRect.right = static_cast<float>(screenSize.x);
        scissorRect.bottom = static_cast<float>(screenSize.y);

        commandList->RSSetScissorRects(1, &scissorRect);

        //TransitionBarrier();

        constexpr FLOAT color[4] = { 0.2f, 0.3f, 0.3f, 1.0f };

        D3D12_CPU_DESCRIPTOR_HANDLE targetView = DirectX12RenderContext::getSingleton().mRenderTargetDescriptorHeap.cpuHandle(mTargetView);
        D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = DirectX12RenderContext::getSingleton().mDepthStencilDescriptorHeap.cpuHandle(mDepthStencilView);

        commandList->ClearRenderTargetView(targetView, color, 0, nullptr);
        commandList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        commandList->OMSetRenderTargets(1, &targetView, false, &depthStencilView);
        /*sDeviceContext->OMSetDepthStencilState(mDepthStencilState, 1);
        sDeviceContext->OMSetBlendState(mBlendState, 0, 0xffffffff);*/

        //sDeviceContext->PSSetSamplers(0, 2, mSamplers);

        RenderTarget::beginIteration(iteration);
    }

    void DirectX12RenderTarget::endIteration(size_t iteration) const
    {
        RenderTarget::endIteration(iteration);
#if !MADGINE_DIRECTX12_USE_SINGLE_COMMAND_LIST
        context()->ExecuteCommandList(context()->mCommandList);
#endif
    }

    void DirectX12RenderTarget::pushAnnotation(const char *tag)
    {
        PIXBeginEvent(context()->mCommandList.mList, PIX_COLOR(255, 255, 255), tag);
    }

    void DirectX12RenderTarget::popAnnotation()
    {
        PIXEndEvent(context()->mCommandList.mList);
    }

    void DirectX12RenderTarget::setRenderSpace(const Rect2i &space)
    {
        ID3D12GraphicsCommandList *commandList = context()->mCommandList.mList;

        D3D12_VIEWPORT viewport;
        viewport.Width = static_cast<float>(space.mSize.x);
        viewport.Height = static_cast<float>(space.mSize.y);
        viewport.TopLeftX = static_cast<float>(space.mTopLeft.x);
        viewport.TopLeftY = static_cast<float>(space.mTopLeft.y);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        commandList->RSSetViewports(1, &viewport);

        D3D12_RECT scissorRect;
        scissorRect.left = static_cast<float>(space.mTopLeft.x);
        scissorRect.top = static_cast<float>(space.mTopLeft.y);
        scissorRect.right = static_cast<float>(space.mTopLeft.x + space.mSize.x);
        scissorRect.bottom = static_cast<float>(space.mTopLeft.y + space.mSize.y);

        commandList->RSSetScissorRects(1, &scissorRect);
    }

    void DirectX12RenderTarget::renderMesh(const GPUMeshData *m, const PipelineInstance *p, const Material *material)
    {
        renderMeshInstanced(1, m, p, material);
        /* ID3D12GraphicsCommandList *commandList = context()->mCommandList.mList;

        const DirectX12MeshData *mesh = static_cast<const DirectX12MeshData *>(m);
        const DirectX12PipelineInstance *pipeline = static_cast<const DirectX12PipelineInstance *>(p);

        mesh->mVertices.bindVertex(commandList, mesh->mVertexSize);

        pipeline->bind(commandList, mesh->mFormat, mesh->mGroupSize);

        if (material)
            bindTextures({ { material->mDiffuseTexture, TextureType_2D } });

        constexpr D3D12_PRIMITIVE_TOPOLOGY modes[] {
            D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
            D3D_PRIMITIVE_TOPOLOGY_LINELIST,
            D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        };

        assert(mesh->mGroupSize > 0 && mesh->mGroupSize <= 3);
        D3D12_PRIMITIVE_TOPOLOGY mode = modes[mesh->mGroupSize - 1];
        commandList->IASetPrimitiveTopology(mode);

        if (mesh->mIndices) {
            mesh->mIndices.bindIndex(commandList);
            commandList->DrawIndexedInstanced(mesh->mElementCount, 1, 0, 0, 0);
        } else {
            commandList->DrawInstanced(mesh->mElementCount, 1, 0, 0);
        }*/
    }

    void DirectX12RenderTarget::renderMeshInstanced(size_t count, const GPUMeshData *m, const PipelineInstance *p, const Material *material)
    {
        ID3D12GraphicsCommandList *commandList = context()->mCommandList.mList;

        const DirectX12PipelineInstance *pipeline = static_cast<const DirectX12PipelineInstance *>(p);
        const DirectX12MeshData *mesh = static_cast<const DirectX12MeshData *>(m);

        if (!pipeline->bind(commandList, m->mFormat, m->mGroupSize))
            return;

        mesh->mVertices.bindVertex(commandList, mesh->mVertexSize);

        if (material)
            bindTextures({ { material->mDiffuseTexture, TextureType_2D } });

        constexpr D3D12_PRIMITIVE_TOPOLOGY modes[] {
            D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
            D3D_PRIMITIVE_TOPOLOGY_LINELIST,
            D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        };

        assert(m->mGroupSize > 0 && m->mGroupSize <= 3);
        D3D12_PRIMITIVE_TOPOLOGY mode = modes[m->mGroupSize - 1];
        commandList->IASetPrimitiveTopology(mode);

        if (mesh->mIndices) {
            mesh->mIndices.bindIndex(commandList);
            commandList->DrawIndexedInstanced(m->mElementCount, count, 0, 0, 0);
        } else {
            commandList->DrawInstanced(m->mElementCount, count, 0, 0);
        }
    }

    void DirectX12RenderTarget::clearDepthBuffer()
    {
        context()->mCommandList.mList->ClearDepthStencilView(DirectX12RenderContext::getSingleton().mDepthStencilDescriptorHeap.cpuHandle(mDepthStencilView), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    }

    void DirectX12RenderTarget::bindTextures(const std::vector<TextureDescriptor> &tex, size_t offset) const
    {
        for (size_t i = 0; i < tex.size(); ++i) {
            if (tex[i].mTextureHandle) {
                D3D12_GPU_DESCRIPTOR_HANDLE handle;
                handle.ptr = tex[i].mTextureHandle;
                context()->mCommandList.mList->SetGraphicsRootDescriptorTable(3 + offset + i, handle);
            }
        }
    }

    DirectX12RenderContext *DirectX12RenderTarget::context() const
    {
        return static_cast<DirectX12RenderContext *>(RenderTarget::context());
    }

    void DirectX12RenderTarget::TransitionBarrier(ID3D12GraphicsCommandList *commandList, ID3D12Resource *res, D3D12_RESOURCE_STATES from, D3D12_RESOURCE_STATES to)
    {
        D3D12_RESOURCE_BARRIER barrierDesc;
        ZeroMemory(&barrierDesc, sizeof(D3D12_RESOURCE_BARRIER));

        barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrierDesc.Transition.pResource = res;
        barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrierDesc.Transition.StateBefore = from;
        barrierDesc.Transition.StateAfter = to;
        barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

        commandList->ResourceBarrier(1, &barrierDesc);
        DX12_CHECK();
    }

    void DirectX12RenderTarget::Transition(ID3D12Resource *res, D3D12_RESOURCE_STATES from, D3D12_RESOURCE_STATES to) const
    {
        TransitionBarrier(context()->mCommandList.mList, res, from, to);
    }

}
}

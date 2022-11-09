#pragma once

#include "Madgine/render/rendercontext.h"
#include "Madgine/render/rendercontextcollector.h"

#include "Modules/threading/threadlocal.h"


#include "util/directx12constantbufferheap.h"
#include "util/directx12descriptorheap.h"

#include "Generic/lambda.h"

#include "Madgine/render/vertexformat.h"

#include "util/directx12buffer.h"

namespace Engine {
namespace Render {

    MADGINE_DIRECTX12_EXPORT ID3D12Device *GetDevice();

    struct DirectX12CommandList {
        ReleasePtr<ID3D12GraphicsCommandList> mList;
        ReleasePtr<ID3D12CommandAllocator> mAllocator;
    };

    struct MADGINE_DIRECTX12_EXPORT DirectX12RenderContext : public RenderContextComponent<DirectX12RenderContext> {
        DirectX12RenderContext(Threading::TaskQueue *queue);
        ~DirectX12RenderContext();

        virtual std::unique_ptr<RenderTarget> createRenderWindow(Window::OSWindow *w, size_t samples) override;
        virtual std::unique_ptr<RenderTarget> createRenderTexture(const Vector2i &size = { 1, 1 }, const RenderTextureConfig &config = {}) override;

        virtual void beginFrame() override;
        virtual void endFrame() override;

        virtual bool supportsMultisampling() const override;

        virtual void pushAnnotation(const char *tag) override;
        virtual void popAnnotation() override;

        void waitForGPU();
        void waitForFence(uint64_t fenceValue);
        bool isFenceComplete(uint64_t fenceValue);

        static DirectX12RenderContext &getSingleton();

        void createRootSignature();
        void ExecuteCommandList(DirectX12CommandList &list, Lambda<void()> dtor = {});

        ReleasePtr<ID3D12CommandAllocator> fetchCommandAllocator(ReleasePtr<ID3D12CommandAllocator> discardAllocator = nullptr, Lambda<void()> dtor = {});

        static std::vector<D3D12_INPUT_ELEMENT_DESC> createVertexLayout(VertexFormat format, size_t instanceDataSize);

        DirectX12DescriptorHeap mDescriptorHeap;
        DirectX12DescriptorHeap mRenderTargetDescriptorHeap;
        DirectX12DescriptorHeap mDepthStencilDescriptorHeap;
        DirectX12ConstantBufferHeap mConstantBufferHeap;
        ReleasePtr<ID3D12CommandQueue> mCommandQueue;
        DirectX12CommandList mCommandList;
        DirectX12CommandList mTempCommandList;
        std::vector<std::tuple<uint64_t, ReleasePtr<ID3D12CommandAllocator>, Lambda<void()>>> mAllocatorPool;
        ReleasePtr<ID3D12RootSignature> mRootSignature;
        ReleasePtr<IDXGIFactory4> mFactory;

        uint64_t mLastCompletedFenceValue = 0;
        uint64_t mNextFenceValue;
        ID3D12Fence *mFence;
        HANDLE mFenceEvent;

        DirectX12Buffer mConstantBuffer;
    };

}
}

REGISTER_TYPE(Engine::Render::DirectX12RenderContext)
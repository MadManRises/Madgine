#pragma once

#include "Madgine/render/rendercontext.h"
#include "Madgine/render/rendercontextcollector.h"

#include "util/directx12constantbufferheap.h"
#include "util/directx12descriptorheap.h"
#include "util/directx12commandlist.h"

#include "Generic/lambda.h"

#include "Madgine/render/vertexformat.h"

#include "util/directx12buffer.h"

namespace Engine {
namespace Render {

    MADGINE_DIRECTX12_EXPORT ID3D12Device *GetDevice();

    struct MADGINE_DIRECTX12_EXPORT DirectX12RenderContext : public RenderContextComponent<DirectX12RenderContext> {
        DirectX12RenderContext(Threading::TaskQueue *queue);
        ~DirectX12RenderContext();

        virtual std::unique_ptr<RenderTarget> createRenderWindow(Window::OSWindow *w, size_t samples) override;
        virtual std::unique_ptr<RenderTarget> createRenderTexture(const Vector2i &size = { 1, 1 }, const RenderTextureConfig &config = {}) override;

        virtual bool beginFrame() override;
        virtual void endFrame() override;

        virtual Threading::Task<void> unloadAllResources() override;

        virtual bool supportsMultisampling() const override;

        uint64_t currentFence();

        bool isFenceComplete(uint64_t fenceValue);

        static DirectX12RenderContext &getSingleton();

        void createRootSignature();

        DirectX12CommandList fetchCommandList(std::string_view name);
        void ExecuteCommandList(ReleasePtr<ID3D12GraphicsCommandList> list, ReleasePtr<ID3D12CommandAllocator> allocator, std::vector<ReleasePtr<ID3D12Pageable>> discardResources);

        static std::vector<D3D12_INPUT_ELEMENT_DESC> createVertexLayout(VertexFormat format, size_t instanceDataSize);

        DirectX12DescriptorHeap mDescriptorHeap;
        DirectX12DescriptorHeap mRenderTargetDescriptorHeap;
        DirectX12DescriptorHeap mDepthStencilDescriptorHeap;
        DirectX12ConstantBufferHeap mConstantBufferHeap;
        ReleasePtr<ID3D12CommandQueue> mCommandQueue;
        std::vector<std::tuple<uint64_t, ReleasePtr<ID3D12CommandAllocator>, std::vector<ReleasePtr<ID3D12Pageable>>>> mAllocatorPool;
        std::vector<ReleasePtr<ID3D12GraphicsCommandList>> mCommandListPool;
        ReleasePtr<ID3D12RootSignature> mRootSignature;
        ReleasePtr<IDXGIFactory4> mFactory;

        uint64_t mLastCompletedFenceValue = 0;
        uint64_t mNextFenceValue;
        ID3D12Fence *mFence;

        DirectX12Buffer mConstantBuffer;
    };

}
}

REGISTER_TYPE(Engine::Render::DirectX12RenderContext)
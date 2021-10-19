#pragma once

#include "Madgine/render/rendercontext.h"
#include "Madgine/render/rendercontextcollector.h"

#include "Modules/threading/threadlocal.h"


#include "util/directx12constantbufferheap.h"
#include "util/directx12descriptorheap.h"

namespace Engine {
namespace Render {

    extern THREADLOCAL(ID3D12Device *) sDevice;

    MADGINE_DIRECTX12_EXPORT ID3D12Device *GetDevice();

    struct DirectX12CommandList {
        ID3D12GraphicsCommandList *mList;
        ID3D12CommandAllocator *mAllocator;
    };

    struct MADGINE_DIRECTX12_EXPORT DirectX12RenderContext : public RenderContextComponent<DirectX12RenderContext> {
        DirectX12RenderContext(Threading::TaskQueue *queue);
        ~DirectX12RenderContext();

        virtual std::unique_ptr<RenderTarget> createRenderWindow(Window::OSWindow *w, size_t samples) override;
        virtual std::unique_ptr<RenderTarget> createRenderTexture(const Vector2i &size = { 1, 1 }, const RenderTextureConfig &config = {}) override;

        virtual void beginFrame() override;

        void waitForGPU();
        void waitForFence(uint64_t fenceValue);
        bool isFenceComplete(uint64_t fenceValue);

        static DirectX12RenderContext &getSingleton();

        void createRootSignature();
        void ExecuteCommandList(DirectX12CommandList &list, std::function<void()> dtor = {});

        ID3D12CommandAllocator *fetchCommandAllocator(ID3D12CommandAllocator *discardAllocator = nullptr, std::function<void()> dtor = {});

        DirectX12DescriptorHeap mDescriptorHeap;
        DirectX12DescriptorHeap mRenderTargetDescriptorHeap;
        DirectX12DescriptorHeap mDepthStencilDescriptorHeap;
        DirectX12ConstantBufferHeap mConstantBufferHeap;
        ID3D12CommandQueue *mCommandQueue;
        DirectX12CommandList mCommandList;
        DirectX12CommandList mTempCommandList;
        std::vector<std::tuple<uint64_t, ID3D12CommandAllocator *, std::function<void()>>> mAllocatorPool;
        ID3D12RootSignature *mRootSignature;
        IDXGIFactory4 *mFactory;

        uint64_t mLastCompletedFenceValue = 0;
        uint64_t mNextFenceValue;
        ID3D12Fence *mFence;
        HANDLE mFenceEvent;
    };

}
}

RegisterType(Engine::Render::DirectX12RenderContext);
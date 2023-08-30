#pragma once

#include "directx12commandlist.h"

namespace Engine {
namespace Render {

    struct DirectX12CommandAllocator {
        DirectX12CommandAllocator(D3D12_COMMAND_LIST_TYPE type, std::string_view name, DirectX12DescriptorHeap *descriptorHeap);

        void setup();

        DirectX12CommandList fetchCommandList();
        void ExecuteCommandList(ReleasePtr<ID3D12GraphicsCommandList> list, ReleasePtr<ID3D12CommandAllocator> allocator, std::vector<Any> discardResources);

        uint64_t currentFence();

        bool isFenceCompleteRelative(uint64_t offset);
        bool isFenceComplete(uint64_t fenceValue);

        void signalFence();

        ID3D12CommandQueue *queue();

    private:
        D3D12_COMMAND_LIST_TYPE mType;
        std::string mName;
        DirectX12DescriptorHeap *mDescriptorHeap;
        ReleasePtr<ID3D12CommandQueue> mCommandQueue;
        std::vector<std::tuple<uint64_t, ReleasePtr<ID3D12CommandAllocator>, std::vector<Any>>> mAllocatorPool;
        std::vector<ReleasePtr<ID3D12GraphicsCommandList>> mCommandListPool;

        uint64_t mLastCompletedFenceValue = 0;
        uint64_t mNextFenceValue;
        ID3D12Fence *mFence;
    };

}
}
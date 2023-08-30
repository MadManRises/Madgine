#include "../directx12lib.h"

#include "../directx12rendercontext.h"
#include "directx12commandallocator.h"

namespace Engine {
namespace Render {

    DirectX12CommandAllocator::DirectX12CommandAllocator(D3D12_COMMAND_LIST_TYPE type, std::string_view name, DirectX12DescriptorHeap *descriptorHeap)
        : mType(type)
        , mName(name)
        , mDescriptorHeap(descriptorHeap)
    {

    }

    void DirectX12CommandAllocator::setup()
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = mType;

        HRESULT hr = GetDevice()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue));
        DX12_CHECK(hr);

        mLastCompletedFenceValue = 5;
        hr = GetDevice()->CreateFence(mLastCompletedFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
        DX12_CHECK(hr);

        mNextFenceValue = mLastCompletedFenceValue + 1;
    }

    DirectX12CommandList DirectX12CommandAllocator::fetchCommandList()
    {
        ReleasePtr<ID3D12CommandAllocator> alloc;

        if (!mAllocatorPool.empty()) {
            auto &[fenceValue, allocator, discard] = mAllocatorPool.front();

            if (isFenceComplete(fenceValue)) {
                alloc = std::move(allocator);
                mAllocatorPool.erase(mAllocatorPool.begin());
                HRESULT hr = alloc->Reset();
                DX12_CHECK(hr);
            }
        }

        if (!alloc) {
            HRESULT hr = GetDevice()->CreateCommandAllocator(mType, IID_PPV_ARGS(&alloc));
            DX12_CHECK(hr);
        }

        ReleasePtr<ID3D12GraphicsCommandList> list;

        if (!mCommandListPool.empty()) {
            list = std::move(mCommandListPool.back());
            mCommandListPool.pop_back();
            HRESULT hr = list->Reset(alloc, nullptr);
            DX12_CHECK(hr);
        } else {
            HRESULT hr = GetDevice()->CreateCommandList(0, mType, alloc, nullptr, IID_PPV_ARGS(&list));
            DX12_CHECK(hr);
        }

        list->SetName(StringUtil::toWString(mName).c_str());

        if (mType == D3D12_COMMAND_LIST_TYPE_DIRECT) {
            ID3D12DescriptorHeap *heap = mDescriptorHeap->resource();
            list->SetDescriptorHeaps(1, &heap);
        }

        return { this, std::move(list), std::move(alloc) };
    }

    void DirectX12CommandAllocator::ExecuteCommandList(ReleasePtr<ID3D12GraphicsCommandList> list, ReleasePtr<ID3D12CommandAllocator> allocator, std::vector<Any> discardResources)
    {
        HRESULT hr = list->Close();
        DX12_CHECK(hr);

        ID3D12CommandList *cList = list;
        mCommandQueue->ExecuteCommandLists(1, &cList);
        DX12_CHECK();

        if (allocator || !discardResources.empty())
            mAllocatorPool.emplace_back(mNextFenceValue, std::move(allocator), std::move(discardResources));

        mCommandListPool.push_back(std::move(list));
    }

    uint64_t DirectX12CommandAllocator::currentFence()
    {
        return mNextFenceValue - 1;
    }

    bool DirectX12CommandAllocator::isFenceCompleteRelative(uint64_t offset)
    {
        return isFenceComplete(mNextFenceValue - 1 - offset);
    }

    bool DirectX12CommandAllocator::isFenceComplete(uint64_t fenceValue)
    {
        // if it's greater than last seen fence value
        // check fence for latest completed value
        if (fenceValue > mLastCompletedFenceValue)
            mLastCompletedFenceValue = std::max(mLastCompletedFenceValue, mFence->GetCompletedValue());

        return fenceValue <= mLastCompletedFenceValue;
    }

    void DirectX12CommandAllocator::signalFence()
    {
        mCommandQueue->Signal(mFence, mNextFenceValue);
        ++mNextFenceValue;
    }

    ID3D12CommandQueue *DirectX12CommandAllocator::queue()
    {
        return mCommandQueue;
    }

}
}
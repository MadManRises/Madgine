#include "../directx12lib.h"

#include "../directx12rendercontext.h"
#include "directx12commandlist.h"

namespace Engine {
namespace Render {

    DirectX12CommandList::DirectX12CommandList(ReleasePtr<ID3D12GraphicsCommandList> list, ReleasePtr<ID3D12CommandAllocator> allocator)
        : mList(std::move(list))
        , mAllocator(std::move(allocator))
    {
    }

    DirectX12CommandList::~DirectX12CommandList()
    {
        reset();
    }

    DirectX12CommandList &DirectX12CommandList::operator=(DirectX12CommandList &&other)
    {
        reset();
        mList = std::move(other.mList);
        mAllocator = std::move(other.mAllocator);
        mAttachedResources = std::move(other.mAttachedResources);
        return *this;
    }

    void DirectX12CommandList::reset()
    {
        if (mList) {
            DirectX12RenderContext::getSingleton().ExecuteCommandList(std::move(mList), std::move(mAllocator), std::move(mAttachedResources));
        }
    }

    DirectX12CommandList::operator ID3D12GraphicsCommandList *()
    {
        return mList;
    }

    ID3D12GraphicsCommandList *DirectX12CommandList::operator->()
    {
        return mList;
    }

    void DirectX12CommandList::attachResource(ReleasePtr<ID3D12Resource> resource)
    {
        mAttachedResources.push_back(std::move(resource));
    }

    void DirectX12CommandList::attachResource(ReleasePtr<ID3D12PipelineState> resource)
    {
        mAttachedResources.push_back(std::move(resource));
    }

    void DirectX12CommandList::Transition(ID3D12Resource *res, D3D12_RESOURCE_STATES from, D3D12_RESOURCE_STATES to) const
    {
        D3D12_RESOURCE_BARRIER barrierDesc;
        ZeroMemory(&barrierDesc, sizeof(D3D12_RESOURCE_BARRIER));

        barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrierDesc.Transition.pResource = res;
        barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrierDesc.Transition.StateBefore = from;
        barrierDesc.Transition.StateAfter = to;
        barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

        mList->ResourceBarrier(1, &barrierDesc);
        DX12_CHECK();
    }

}
}
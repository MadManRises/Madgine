#pragma once

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12CommandList {
        DirectX12CommandList() = default;
        DirectX12CommandList(ReleasePtr<ID3D12GraphicsCommandList> list, ReleasePtr<ID3D12CommandAllocator> allocator);
        ~DirectX12CommandList();

        DirectX12CommandList &operator=(DirectX12CommandList &&);

        void reset();

        operator ID3D12GraphicsCommandList *();
        ID3D12GraphicsCommandList *operator->();

        void attachResource(ReleasePtr<ID3D12Resource> resource);
        void attachResource(ReleasePtr<ID3D12PipelineState> resource);

        void Transition(ID3D12Resource *res, D3D12_RESOURCE_STATES from, D3D12_RESOURCE_STATES to) const;

    private:
        ReleasePtr<ID3D12GraphicsCommandList> mList;
        ReleasePtr<ID3D12CommandAllocator> mAllocator;
        std::vector<ReleasePtr<ID3D12Pageable>> mAttachedResources;
    };

}
}
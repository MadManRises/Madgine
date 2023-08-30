#include "../directx12lib.h"

#include "directx12heapallocator.h"

#include "../directx12rendercontext.h"

namespace Engine {
namespace Render {

    DirectX12HeapAllocator::DirectX12HeapAllocator()
    {
    }

    Block DirectX12HeapAllocator::allocate(size_t size)
    {
        GPUPtr<void> ptr;

        CD3DX12_HEAP_DESC heapDesc = CD3DX12_HEAP_DESC { size, D3D12_HEAP_TYPE_DEFAULT };

        HRESULT hr = GetDevice()->CreateHeap(
            &heapDesc,
            IID_PPV_ARGS(&mHeaps.emplace_back()));
        DX12_CHECK(hr);
        mHeaps.back()->SetName(L"Persistent Heap");

        ptr.mBuffer = mHeaps.size();
        ptr.mOffset = 0;

        return { reinterpret_cast<void *&>(ptr), size };
    }

    void DirectX12HeapAllocator::deallocate(Block block)
    {
        GPUPtr<void> ptr = reinterpret_cast<GPUPtr<void> &>(block.mAddress);
        assert(ptr.mOffset == 0 && ptr.mBuffer != 0);

        size_t index = ptr.mBuffer - 1;
        assert(mHeaps[index]->GetDesc().SizeInBytes == block.mSize);
        mHeaps[index].reset();
    }

    ID3D12Heap *DirectX12HeapAllocator::heap(size_t index)
    {
        return mHeaps[index - 1];
    }

    DirectX12MappedHeapAllocator::DirectX12MappedHeapAllocator(D3D12_HEAP_TYPE type)
        : mType(type)
    {
    }

    Block DirectX12MappedHeapAllocator::allocate(size_t size)
    {
        auto heapDesc = CD3DX12_HEAP_PROPERTIES(mType);
        auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

        Page &page = mPages.emplace_back();

        HRESULT hr = GetDevice()->CreateCommittedResource(
            &heapDesc,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&page.mResource));
        DX12_CHECK(hr);

        page.mResource->Map(0, nullptr, &page.mMappedAddress);

        return { page.mMappedAddress, size };
    }

    void DirectX12MappedHeapAllocator::deallocate(Block block)
    {
        auto it = std::ranges::find(mPages, block.mAddress, &Page::mMappedAddress);
        assert(it != mPages.end());

        assert(it->mResource->GetDesc().Width == block.mSize);
        mPages.erase(it);
    }

    std::pair<ID3D12Resource *, size_t> DirectX12MappedHeapAllocator::resolve(void *ptr)
    {
        auto it = std::ranges::find_if(mPages, [=](const Page &page) {
            return page.mMappedAddress <= ptr && ptr < static_cast<std::byte *>(page.mMappedAddress) + page.mResource->GetDesc().Width;
        });
        assert(it != mPages.end());
        return { it->mResource, static_cast<std::byte *>(ptr) - static_cast<std::byte *>(it->mMappedAddress) };
    }

}
}
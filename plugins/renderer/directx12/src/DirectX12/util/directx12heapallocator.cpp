#include "../directx12lib.h"

#include "directx12heapallocator.h"

#include "../directx12rendercontext.h"

namespace Engine {
namespace Render {

    DirectX12HeapAllocator::DirectX12HeapAllocator(DirectX12DescriptorHeap &descHeap)
        : mDescriptorHeap(descHeap)
    {
    }

    void DirectX12HeapAllocator::setup(size_t count)
    {
        mDescriptors = mDescriptorHeap.allocate(count);
    }

    void DirectX12HeapAllocator::reset()
    {
        mHeaps.clear();
        mDescriptorHeap.deallocate(mDescriptors);
    }

    Block DirectX12HeapAllocator::allocate(size_t size, size_t alignment)
    {
        DX12_LOG("Allocating DX12 heap memory: (size: " << size << ", " << alignment << ")");

        GPUPtr<void> ptr;

        CD3DX12_HEAP_DESC heapDesc = CD3DX12_HEAP_DESC { size, D3D12_HEAP_TYPE_DEFAULT };
        
        Heap &heap = mHeaps.emplace_back();

        HRESULT hr = GetDevice()->CreateHeap(
            &heapDesc,
            IID_PPV_ARGS(&heap.mHeap));
        DX12_CHECK(hr);
        mHeaps.back().mHeap->SetName(L"Persistent Heap");

        auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
        hr = GetDevice()->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc, 
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&heap.mReservedResource));
        DX12_CHECK(hr);
        heap.mReservedResource->SetName(L"Persistent Resource");

        D3D12_SHADER_RESOURCE_VIEW_DESC desc {};
        desc.Format = DXGI_FORMAT_R32_TYPELESS;
        desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        desc.Buffer.FirstElement = 0;
        desc.Buffer.StructureByteStride = 0;
        desc.Buffer.NumElements = size / 4;
        desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
        mDescriptorHeap.addShaderResourceView(mDescriptors + mHeaps.size() - 1, mHeaps.back().mReservedResource, &desc);

        ptr.mBuffer = mHeaps.size();
        ptr.mOffset = 0;

        return { reinterpret_cast<void *&>(ptr), size };
    }

    void DirectX12HeapAllocator::deallocate(Block block)
    {
        GPUPtr<void> ptr = reinterpret_cast<GPUPtr<void> &>(block.mAddress);
        assert(ptr.mOffset == 0 && ptr.mBuffer != 0);

        size_t index = ptr.mBuffer - 1;
        assert(mHeaps[index].mHeap->GetDesc().SizeInBytes == block.mSize);
        mHeaps[index].mReservedResource.reset();
        mHeaps[index].mHeap.reset();        
    }

    ID3D12Heap *DirectX12HeapAllocator::heap(size_t index)
    {
        return mHeaps[index - 1].mHeap;
    }

    std::pair<ID3D12Resource*, size_t> DirectX12HeapAllocator::resolve(void *ptr)
    {
        GPUPtr<void> &gpuPtr = reinterpret_cast<GPUPtr<void>&>(ptr);
        return { mHeaps[gpuPtr.mBuffer - 1].mReservedResource, gpuPtr.mOffset };
    }

    D3D12_GPU_DESCRIPTOR_HANDLE DirectX12HeapAllocator::descriptorTable() const
    {
        return mDescriptorHeap.gpuHandle(mDescriptors);
    }

    DirectX12MappedHeapAllocator::DirectX12MappedHeapAllocator(D3D12_HEAP_TYPE type)
        : mType(type)
    {
    }

    void DirectX12MappedHeapAllocator::reset()
    {
        mPages.clear();
    }

    Block DirectX12MappedHeapAllocator::allocate(size_t size, size_t alignment)
    {
        auto heapDesc = CD3DX12_HEAP_PROPERTIES(mType);
        auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

        Page &page = mPages.emplace_back();

        HRESULT hr = GetDevice()->CreateCommittedResource(
            &heapDesc,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&page.mResource));
        DX12_CHECK(hr);
        page.mResource->SetName(L"Temp Resource");

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
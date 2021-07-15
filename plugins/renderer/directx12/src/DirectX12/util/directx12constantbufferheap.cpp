#include "../directx12lib.h"

#include "directx12constantbufferheap.h"

#include "../directx12rendercontext.h"

#include "Generic/bytebuffer.h"

namespace Engine {
namespace Render {

    DirectX12ConstantBufferHeap::DirectX12ConstantBufferHeap(DirectX12DescriptorHeap *descriptorHeap, size_t size) : 
        mDescriptorHeap(descriptorHeap), mSize(size)
    {
        CD3DX12_HEAP_PROPERTIES heapProperties { D3D12_HEAP_TYPE_UPLOAD };

        CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

        HRESULT hr = sDevice->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&mHeap));
        DX12_CHECK(hr);
    }

    DirectX12ConstantBufferHeap::~DirectX12ConstantBufferHeap()
    {
        if (mHeap) {
            mHeap->Release();
            mHeap = nullptr;
        }
    }

    DirectX12ConstantBufferHeap &DirectX12ConstantBufferHeap::operator=(DirectX12ConstantBufferHeap &&other)
    {
        std::swap(mDescriptorHeap, other.mDescriptorHeap);
        std::swap(mHeap, other.mHeap);
        std::swap(mSize, other.mSize);
        std::swap(mOffset, other.mOffset);
        return *this;
    }

    OffsetPtr DirectX12ConstantBufferHeap::allocate(size_t size)
    {
        assert(mOffset + size <= mSize);
        OffsetPtr offset = mOffset;
        mOffset += size;
        return offset;
    }

    WritableByteBuffer DirectX12ConstantBufferHeap::map(OffsetPtr offset, size_t size)
    {
        struct UnmapDeleter {
            ID3D12Resource *mBuffer;
            D3D12_RANGE mRange;

            void operator()(void *p)
            {
                mBuffer->Unmap(0, &mRange);
            }
        };

        D3D12_RANGE range { offset.offset(), (offset + size).offset() };
        void *data;
        HRESULT hr = mHeap->Map(0, &range, &data);        
        DX12_CHECK(hr);

        std::unique_ptr<void, UnmapDeleter> dataBuffer { data, { mHeap, range } };

        return { std::move(dataBuffer), size };
    }

    DirectX12DescriptorHeap *DirectX12ConstantBufferHeap::descriptorHeap() const
    {
        return mDescriptorHeap;
    }

    ID3D12Resource *DirectX12ConstantBufferHeap::resource() const
    {
        return mHeap;
    }

    D3D12_GPU_VIRTUAL_ADDRESS DirectX12ConstantBufferHeap::address(OffsetPtr offset)
    {
        return reinterpret_cast<uintptr_t>(reinterpret_cast<std::byte*>(mHeap->GetGPUVirtualAddress()) + offset);
    }

}
}
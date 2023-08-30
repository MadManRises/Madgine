#include "../directx12lib.h"

#include "directx12constantbufferheap.h"

#include "../directx12rendercontext.h"

#include "Generic/bytebuffer.h"

namespace Engine {
namespace Render {

    DirectX12ConstantBufferHeap::DirectX12ConstantBufferHeap(size_t size)
        : mSize(size)
    {
        CD3DX12_HEAP_PROPERTIES persistentHeapProperties { D3D12_HEAP_TYPE_DEFAULT };
        CD3DX12_HEAP_PROPERTIES tempHeapProperties { D3D12_HEAP_TYPE_UPLOAD };

        CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

        HRESULT hr = GetDevice()->CreateCommittedResource(
            &persistentHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&mPersistentHeap));
        DX12_CHECK(hr);
        mPersistentHeap->SetName(L"Persistent Heap");

        hr = GetDevice()->CreateCommittedResource(
            &tempHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&mTempHeap));
        DX12_CHECK(hr);
        mTempHeap->SetName(L"Temp Heap");
    }

    DirectX12ConstantBufferHeap &DirectX12ConstantBufferHeap::operator=(DirectX12ConstantBufferHeap &&other)
    {
        std::swap(mTempHeap, other.mTempHeap);
        std::swap(mPersistentHeap, other.mPersistentHeap);
        std::swap(mSize, other.mSize);
        std::swap(mOffsetTemp, other.mOffsetTemp);
        std::swap(mOffsetPersistent, other.mOffsetPersistent);
        return *this;
    }

    D3D12_GPU_VIRTUAL_ADDRESS DirectX12ConstantBufferHeap::allocateTemp(size_t size)
    {
        /* for (auto it = mFreeListTemp.begin(); it != mFreeListTemp.end(); ++it) {
            if (it->second == size) {
                OffsetPtr offset = it->first;
                mFreeListTemp.erase(it);
                return offset;
            } else if (it->second > size) {
                it->second -= size;
                return it->first + it->second;
            }
        }
        */
        if (mOffsetTemp + size > mSize) {
            mOffsetTemp = OffsetPtr { 0 };
        }
        OffsetPtr offset = mOffsetTemp;
        mOffsetTemp += size;
        return offset.offset() + mTempHeap->GetGPUVirtualAddress();
    }

    void DirectX12ConstantBufferHeap::deallocateTemp(D3D12_GPU_VIRTUAL_ADDRESS ptr, size_t size)
    {
        //mFreeListTemp.push_back({ ptr, size });
    }

    D3D12_GPU_VIRTUAL_ADDRESS DirectX12ConstantBufferHeap::allocatePersistent(size_t size)
    {
        for (auto it = mFreeListPersistent.begin(); it != mFreeListPersistent.end(); ++it) {
            if (it->second == size) {
                D3D12_GPU_VIRTUAL_ADDRESS ptr = it->first;
                mFreeListPersistent.erase(it);
                return ptr;
            } else if (it->second > size) {
                it->second -= size;
                return it->first + it->second;
            }
        }

        assert(mOffsetPersistent + size <= mSize);
        OffsetPtr offset = mOffsetPersistent;
        mOffsetPersistent += size;
        return offset.offset() + mPersistentHeap->GetGPUVirtualAddress();
    }

    void DirectX12ConstantBufferHeap::deallocatePersistent(D3D12_GPU_VIRTUAL_ADDRESS ptr, size_t size)
    {
        mFreeListPersistent.push_back({ ptr, size });
    }

    WritableByteBuffer DirectX12ConstantBufferHeap::map(D3D12_GPU_VIRTUAL_ADDRESS ptr, size_t size)
    {
        struct UnmapDeleter {
            ID3D12Resource *mBuffer;
            D3D12_RANGE mRange;

            void operator()(void *p)
            {
                mBuffer->Unmap(0, &mRange);
            }
        };

        D3D12_GPU_VIRTUAL_ADDRESS baseAddress = mTempHeap->GetGPUVirtualAddress();
        D3D12_RANGE range { ptr - baseAddress, ptr - baseAddress + size };
        void *data;
        HRESULT hr = mTempHeap->Map(0, &range, &data);
        DX12_CHECK(hr);

        std::unique_ptr<void, UnmapDeleter> dataBuffer { static_cast<std::byte *>(data) + ptr - baseAddress, { mTempHeap, range } };

        return { std::move(dataBuffer), size };
    }

    void DirectX12ConstantBufferHeap::setData(D3D12_GPU_VIRTUAL_ADDRESS ptr, const ByteBuffer &data)
    {
        auto heapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(data.mSize);
        ReleasePtr<ID3D12Resource> uploadHeap;
        HRESULT hr = GetDevice()->CreateCommittedResource(
            &heapDesc,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&uploadHeap));
        DX12_CHECK(hr);

        void *dest;
        hr = uploadHeap->Map(0, nullptr, &dest);
        DX12_CHECK(hr);

        memcpy(dest, data.mData, data.mSize);

        uploadHeap->Unmap(0, nullptr);

        DirectX12CommandList list = DirectX12RenderContext::getSingleton().fetchCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);

        list.Transition(mPersistentHeap, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST);        
        list->CopyBufferRegion(mPersistentHeap, ptr - mPersistentHeap->GetGPUVirtualAddress(), uploadHeap, 0, data.mSize);

        list.Transition(mPersistentHeap, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);        

        list.attachResource(std::move(uploadHeap));
    }

    ID3D12Resource *DirectX12ConstantBufferHeap::resourceTemp() const
    {
        return mTempHeap;
    }

    ID3D12Resource *DirectX12ConstantBufferHeap::resourcePersistent() const
    {
        return mPersistentHeap;
    }

}
}
#include "../directx12lib.h"

#include "directx12constantbufferheap.h"

#include "../directx12rendercontext.h"

#include "Generic/bytebuffer.h"

namespace Engine {
namespace Render {

    DirectX12ConstantBufferHeap::DirectX12ConstantBufferHeap(DirectX12DescriptorHeap *descriptorHeap, size_t size)
        : mDescriptorHeap(descriptorHeap)
        , mSize(size)
    {
        CD3DX12_HEAP_PROPERTIES persistentHeapProperties { D3D12_HEAP_TYPE_DEFAULT };
        CD3DX12_HEAP_PROPERTIES tempHeapProperties { D3D12_HEAP_TYPE_UPLOAD };

        CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

        HRESULT hr = sDevice->CreateCommittedResource(
            &persistentHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&mPersistentHeap));
        DX12_CHECK(hr);
        mPersistentHeap->SetName(L"Persistent Heap");

        hr = sDevice->CreateCommittedResource(
            &tempHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&mTempHeap));
        DX12_CHECK(hr);
        mTempHeap->SetName(L"Temp Heap");
    }

    DirectX12ConstantBufferHeap::~DirectX12ConstantBufferHeap()
    {
        if (mTempHeap) {
            mTempHeap->Release();
            mTempHeap = nullptr;
        }
        if (mPersistentHeap) {
            mPersistentHeap->Release();
            mPersistentHeap = nullptr;
        }
    }

    DirectX12ConstantBufferHeap &DirectX12ConstantBufferHeap::operator=(DirectX12ConstantBufferHeap &&other)
    {
        std::swap(mDescriptorHeap, other.mDescriptorHeap);
        std::swap(mTempHeap, other.mTempHeap);
        std::swap(mPersistentHeap, other.mPersistentHeap);
        std::swap(mSize, other.mSize);
        std::swap(mOffsetTemp, other.mOffsetTemp);
        std::swap(mOffsetPersistent, other.mOffsetPersistent);
        return *this;
    }

    OffsetPtr DirectX12ConstantBufferHeap::allocateTemp(size_t size)
    {
        for (auto it = mFreeListTemp.begin(); it != mFreeListTemp.end(); ++it) {
            if (it->second == size) {
                OffsetPtr offset = it->first;
                mFreeListTemp.erase(it);
                return offset;
            } else if (it->second > size) {
                it->second -= size;
                return it->first;
            }
        }

        assert(mOffsetTemp + size <= mSize);
        OffsetPtr offset = mOffsetTemp;
        mOffsetTemp += size;
        return offset;
    }

    void DirectX12ConstantBufferHeap::deallocateTemp(OffsetPtr ptr, size_t size)
    {
        mFreeListTemp.push_back({ ptr, size });
    }

    OffsetPtr DirectX12ConstantBufferHeap::allocatePersistent(size_t size)
    {
        for (auto it = mFreeListPersistent.begin(); it != mFreeListPersistent.end(); ++it) {
            if (it->second == size) {
                OffsetPtr offset = it->first;
                mFreeListPersistent.erase(it);
                return offset;
            } else if (it->second > size) {
                it->second -= size;
                return it->first;
            }
        }

        assert(mOffsetPersistent + size <= mSize);
        OffsetPtr offset = mOffsetPersistent;
        mOffsetPersistent += size;
        return offset;
    }

    void DirectX12ConstantBufferHeap::deallocatePersistent(OffsetPtr ptr, size_t size)
    {
        mFreeListPersistent.push_back({ ptr, size });
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
        HRESULT hr = mTempHeap->Map(0, &range, &data);
        DX12_CHECK(hr);

        std::unique_ptr<void, UnmapDeleter> dataBuffer { data, { mTempHeap, range } };

        return { std::move(dataBuffer), size };
    }

    void DirectX12ConstantBufferHeap::setData(OffsetPtr offset, const ByteBuffer &data)
    {
        auto heapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(data.mSize);
        ID3D12Resource *uploadHeap;
        HRESULT hr = sDevice->CreateCommittedResource(
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

        auto transition = CD3DX12_RESOURCE_BARRIER::Transition(mPersistentHeap, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST);
        DirectX12RenderContext::getSingleton().mTempCommandList.mList->ResourceBarrier(1, &transition);
        DirectX12RenderContext::getSingleton().mTempCommandList.mList->CopyBufferRegion(mPersistentHeap, offset.offset(), uploadHeap, 0, data.mSize);
        
        auto transition2 = CD3DX12_RESOURCE_BARRIER::Transition(mPersistentHeap, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
        DirectX12RenderContext::getSingleton().mTempCommandList.mList->ResourceBarrier(1, &transition2);

        DirectX12RenderContext::getSingleton().ExecuteCommandList(DirectX12RenderContext::getSingleton().mTempCommandList, [uploadHeap]() {
            uploadHeap->Release();
        });
    }

    DirectX12DescriptorHeap *DirectX12ConstantBufferHeap::descriptorHeap() const
    {
        return mDescriptorHeap;
    }

    ID3D12Resource *DirectX12ConstantBufferHeap::resourceTemp() const
    {
        return mTempHeap;
    }

    ID3D12Resource *DirectX12ConstantBufferHeap::resourcePersistent() const
    {
        return mPersistentHeap;
    }

    D3D12_GPU_VIRTUAL_ADDRESS DirectX12ConstantBufferHeap::addressTemp(OffsetPtr offset)
    {
        return reinterpret_cast<uintptr_t>(reinterpret_cast<std::byte *>(mTempHeap->GetGPUVirtualAddress()) + offset);
    }

    D3D12_GPU_VIRTUAL_ADDRESS DirectX12ConstantBufferHeap::addressPersistent(OffsetPtr offset)
    {
        return reinterpret_cast<uintptr_t>(reinterpret_cast<std::byte *>(mPersistentHeap->GetGPUVirtualAddress()) + offset);
    }

}
}
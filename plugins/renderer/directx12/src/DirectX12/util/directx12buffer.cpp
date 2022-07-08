#include "../directx12lib.h"

#include "directx12buffer.h"

#include "../directx12rendercontext.h"

#include "Generic/bytebuffer.h"

#include "Generic/align.h"

namespace Engine {
namespace Render {

    DirectX12Buffer::DirectX12Buffer(const ByteBuffer &data)
        : mSize(data.mSize)
        , mPersistent(data.mData)
    {
        if (mSize > 0) {
            DirectX12ConstantBufferHeap &heap = DirectX12RenderContext::getSingleton().mConstantBufferHeap;

            size_t actualSize = alignTo(data.mSize, 256);

            mOffset = mPersistent ? heap.allocatePersistent(actualSize) : heap.allocateTemp(actualSize);

            D3D12_CONSTANT_BUFFER_VIEW_DESC bufferDesc;
            ZeroMemory(&bufferDesc, sizeof(D3D12_CONSTANT_BUFFER_VIEW_DESC));

            bufferDesc.SizeInBytes = actualSize;
            bufferDesc.BufferLocation = mPersistent ? heap.addressPersistent(mOffset) : heap.addressTemp(mOffset);

            mHandle = heap.descriptorHeap()->allocate();
            sDevice->CreateConstantBufferView(&bufferDesc, heap.descriptorHeap()->cpuHandle(mHandle));

            DX12_CHECK();

            setData(data);
        }
    }

    DirectX12Buffer::DirectX12Buffer(DirectX12Buffer &&other)
        : mSize(std::exchange(other.mSize, 0))
        , mHandle(std::exchange(other.mHandle, {}))
        , mOffset(std::exchange(other.mOffset, {}))
        , mPersistent(std::exchange(other.mPersistent, false))
    {
    }

    DirectX12Buffer::~DirectX12Buffer()
    {
        reset();
    }

    DirectX12Buffer &DirectX12Buffer::operator=(DirectX12Buffer &&other)
    {
        std::swap(mSize, other.mSize);
        std::swap(mHandle, other.mHandle);
        std::swap(mOffset, other.mOffset);
        std::swap(mPersistent, other.mPersistent);
        return *this;
    }

    DirectX12Buffer::operator bool() const
    {
        return static_cast<bool>(mHandle);
    }

    void DirectX12Buffer::bindVertex(UINT stride) const
    {
        D3D12_VERTEX_BUFFER_VIEW view;
        view.BufferLocation = mPersistent ? DirectX12RenderContext::getSingleton().mConstantBufferHeap.addressPersistent(mOffset) : DirectX12RenderContext::getSingleton().mConstantBufferHeap.addressTemp(mOffset);
        view.SizeInBytes = mSize;
        view.StrideInBytes = stride;
        DirectX12RenderContext::getSingleton().mCommandList.mList->IASetVertexBuffers(0, 1, &view);
        DX12_LOG("Bind Vertex Buffer -> " << mBuffer);
    }

    void DirectX12Buffer::bindIndex() const
    {
        D3D12_INDEX_BUFFER_VIEW view;
        view.BufferLocation = mPersistent ? DirectX12RenderContext::getSingleton().mConstantBufferHeap.addressPersistent(mOffset) : DirectX12RenderContext::getSingleton().mConstantBufferHeap.addressTemp(mOffset);
        view.SizeInBytes = mSize;
        view.Format = DXGI_FORMAT_R32_UINT;
        DirectX12RenderContext::getSingleton().mCommandList.mList->IASetIndexBuffer(&view);
        DX12_LOG("Bind Index Buffer -> " << mBuffer);
    }

    void DirectX12Buffer::reset()
    {
        if (mHandle) {
            DirectX12ConstantBufferHeap &heap = DirectX12RenderContext::getSingleton().mConstantBufferHeap;

            heap.descriptorHeap()->deallocate(mHandle);
            mHandle.reset();

            size_t actualSize = alignTo(mSize, 256);

            mPersistent ? heap.deallocatePersistent(mOffset, actualSize) : heap.deallocateTemp(mOffset, actualSize);
            mOffset.reset();
            mSize = 0;
        }
    }

    void DirectX12Buffer::setData(const ByteBuffer &data)
    {
        DirectX12ConstantBufferHeap &heap = DirectX12RenderContext::getSingleton().mConstantBufferHeap;
        if (mSize != data.mSize)
            *this = { data };
        else if (data.mData)
            heap.setData(mOffset, data);
    }

    void DirectX12Buffer::resize(size_t size)
    {
        setData({ nullptr, size });
    }

    WritableByteBuffer DirectX12Buffer::mapData()
    {
        return DirectX12RenderContext::getSingleton().mConstantBufferHeap.map(mOffset, mSize);
    }

    OffsetPtr DirectX12Buffer::handle()
    {
        return mHandle;
    }

    D3D12_GPU_VIRTUAL_ADDRESS DirectX12Buffer::gpuAddress() const
    {
        DirectX12ConstantBufferHeap &heap = DirectX12RenderContext::getSingleton().mConstantBufferHeap;
        return mPersistent ? heap.addressPersistent(mOffset) : heap.addressTemp(mOffset);
    }

}
}
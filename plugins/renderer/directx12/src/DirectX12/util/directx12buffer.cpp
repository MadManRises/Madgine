#include "../directx12lib.h"

#include "directx12buffer.h"

#include "../directx12rendercontext.h"

#include "Generic/bytebuffer.h"

#include "Generic/align.h"

namespace Engine {
namespace Render {

    DirectX12Buffer::DirectX12Buffer(DirectX12Buffer &&other)
        : mAddress(std::exchange(other.mAddress, 0))
        , mSize(std::exchange(other.mSize, 0))
        , mIsPersistent(std::exchange(other.mIsPersistent, false))
    {
    }

    DirectX12Buffer::DirectX12Buffer(const ByteBuffer &data)
    {
        setData(data);
    }

    DirectX12Buffer::~DirectX12Buffer()
    {
        reset();
    }

    DirectX12Buffer &DirectX12Buffer::operator=(DirectX12Buffer &&other)
    {
        std::swap(mAddress, other.mAddress);
        std::swap(mSize, other.mSize);
        std::swap(mIsPersistent, other.mIsPersistent);
        return *this;
    }

    DirectX12Buffer::operator bool() const
    {
        return mSize > 0;
    }

    void DirectX12Buffer::bindVertex(ID3D12GraphicsCommandList *commandList, UINT stride, size_t index) const
    {
        D3D12_VERTEX_BUFFER_VIEW view;
        view.BufferLocation = mAddress;
        view.SizeInBytes = mSize;
        view.StrideInBytes = stride;
        commandList->IASetVertexBuffers(index, 1, &view);
        DX12_LOG("Bind Vertex Buffer -> " << mAddress);
    }

    void DirectX12Buffer::bindIndex(ID3D12GraphicsCommandList *commandList) const
    {
        D3D12_INDEX_BUFFER_VIEW view;
        view.BufferLocation = mAddress;
        view.SizeInBytes = mSize;
        view.Format = DXGI_FORMAT_R32_UINT;
        commandList->IASetIndexBuffer(&view);
        DX12_LOG("Bind Index Buffer -> " << mAddress);
    }

    void DirectX12Buffer::reset(size_t size)
    {
        if (mAddress && mIsPersistent) {
            DirectX12ConstantBufferHeap &heap = DirectX12RenderContext::getSingleton().mConstantBufferHeap;

            heap.deallocatePersistent(mAddress, mSize);
        }

        mIsPersistent = false;
        mAddress = 0;
        mSize = size;
    }

    void DirectX12Buffer::setData(const ByteBuffer &data)
    {
        size_t expectedSize = alignTo(data.mSize, 256);
        if (data.mData) {
            DirectX12ConstantBufferHeap &heap = DirectX12RenderContext::getSingleton().mConstantBufferHeap;

            if (!mIsPersistent || mSize != expectedSize) {
                reset(expectedSize);

                mAddress = heap.allocatePersistent(expectedSize);
            }

            mIsPersistent = true;
            heap.setData(mAddress, data);
        } else {
            reset(expectedSize);
        }
    }

    WritableByteBuffer DirectX12Buffer::mapData(size_t size)
    {
        DirectX12ConstantBufferHeap &heap = DirectX12RenderContext::getSingleton().mConstantBufferHeap;

        reset(alignTo(size, 256));

        mAddress = heap.allocateTemp(mSize);

        return heap.map(mAddress, mSize);
    }

    WritableByteBuffer DirectX12Buffer::mapData()
    {
        return mapData(mSize);
    }

    D3D12_GPU_VIRTUAL_ADDRESS DirectX12Buffer::gpuAddress() const
    {
        return mAddress;
    }

}
}
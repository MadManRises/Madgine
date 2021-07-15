#include "../directx12lib.h"

#include "directx12buffer.h"

#include "../directx12rendercontext.h"

#include "directx12vertexarray.h"

#include "Generic/bytebuffer.h"

#include "Generic/align.h"

namespace Engine {
namespace Render {

    DirectX12Buffer::DirectX12Buffer(size_t size)
        : mSize(size)
    {       
        if (size > 0) {
            DirectX12ConstantBufferHeap &heap = DirectX12RenderContext::getSingleton().mConstantBufferHeap;

            size_t actualSize = alignTo(size, 256);

            mOffset = heap.allocate(actualSize);

            D3D12_CONSTANT_BUFFER_VIEW_DESC bufferDesc;
            ZeroMemory(&bufferDesc, sizeof(D3D12_CONSTANT_BUFFER_VIEW_DESC));

            bufferDesc.SizeInBytes = actualSize;
            bufferDesc.BufferLocation = heap.address(mOffset);

            mHandle = heap.descriptorHeap()->allocate();
            sDevice->CreateConstantBufferView(&bufferDesc, heap.descriptorHeap()->cpuHandle(mHandle));

            DX12_CHECK();
        }
    }

    DirectX12Buffer::DirectX12Buffer(const ByteBuffer &data)
        : DirectX12Buffer(data.mSize)
    {
        setData(data);
    }

    DirectX12Buffer::DirectX12Buffer(DirectX12Buffer &&other)
        : mSize(std::exchange(other.mSize, 0))
        , mHandle(std::exchange(other.mHandle, {}))
        , mOffset(std::exchange(other.mOffset, {}))
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
        return *this;
    }

    DirectX12Buffer::operator bool() const
    {
        return true;
    }

    void DirectX12Buffer::bindVertex(UINT stride) const
    {
        /*UINT offset = 0;
        sDeviceContext->IASetVertexBuffers(0, 1, &mBuffer, &stride, &offset);*/
        DX12_LOG("Bind Vertex Buffer -> " << mBuffer);
        DirectX12VertexArray::onBindVBO(this);
    }

    void DirectX12Buffer::bindIndex() const
    {
        //sDeviceContext->IASetIndexBuffer(mBuffer, DXGI_FORMAT_R16_UINT, 0);
        DX12_LOG("Bind Index Buffer -> " << mBuffer);
        DirectX12VertexArray::onBindEBO(this);
    }

    void DirectX12Buffer::reset()
    {
    }

    void DirectX12Buffer::setData(const ByteBuffer &data)
    {
        DirectX12ConstantBufferHeap &heap = DirectX12RenderContext::getSingleton().mConstantBufferHeap;
        if (mSize != data.mSize)
            *this = { data };
        else {
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
            
            D3D12_SUBRESOURCE_DATA dataDesc;
            ZeroMemory(&dataDesc, sizeof(D3D12_SUBRESOURCE_DATA));
            dataDesc.pData = data.mData;
            UpdateSubresources(DirectX12RenderContext::getSingleton().mTempCommandList.mList, heap.resource(), uploadHeap, mOffset.offset(), 0, 0, &dataDesc);            
        
            DirectX12RenderContext::getSingleton().ExecuteCommandList(DirectX12RenderContext::getSingleton().mTempCommandList);

            uploadHeap->Release();
        }
    }

    void DirectX12Buffer::resize(size_t size)
    {
        if (mSize != size) {
            *this = { size };
        }
    }

    WritableByteBuffer DirectX12Buffer::mapData()
    {
        return DirectX12RenderContext::getSingleton().mConstantBufferHeap.map(mOffset, mSize);
    }

    OffsetPtr DirectX12Buffer::handle()
    {
        return mHandle;
    }

}
}
#include "../directx11lib.h"

#include "directx11buffer.h"

#include "../directx11rendercontext.h"

#include "directx11vertexarray.h"

#include "Generic/bytebuffer.h"

namespace Engine {
namespace Render {

    DirectX11Buffer::DirectX11Buffer(UINT bind, size_t size)
        : mSize(size)
        , mBind(bind)
    {
        if (size > 0) {
            D3D11_BUFFER_DESC bufferDesc;
            ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

            bufferDesc.BindFlags = bind;
            bufferDesc.ByteWidth = size;
            bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

            HRESULT hr = sDevice->CreateBuffer(&bufferDesc, nullptr, &mBuffer);
            DX11_CHECK(hr);
        }
    }

    DirectX11Buffer::DirectX11Buffer(UINT bind, const ByteBuffer &data)
        : mSize(data.mSize)
        , mBind(bind)
    {
        assert(data.mData);
        assert(data.mSize);
        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

        bufferDesc.BindFlags = bind;
        bufferDesc.ByteWidth = data.mSize;        
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;

        D3D11_SUBRESOURCE_DATA subData;
        ZeroMemory(&subData, sizeof(D3D11_SUBRESOURCE_DATA));
        subData.pSysMem = data.mData;
        subData.SysMemPitch = 0;
        subData.SysMemSlicePitch = 0;

        HRESULT hr = sDevice->CreateBuffer(&bufferDesc, &subData, &mBuffer);
        DX11_CHECK(hr);
    }

    DirectX11Buffer::DirectX11Buffer(DirectX11Buffer &&other)
        : mSize(std::exchange(other.mSize, 0))
        , mBind(std::exchange(other.mBind, 0))
        , mBuffer(std::exchange(other.mBuffer, nullptr))
    {
    }

    DirectX11Buffer::~DirectX11Buffer()
    {
        reset();
    }

    DirectX11Buffer &DirectX11Buffer::operator=(DirectX11Buffer &&other)
    {
        std::swap(mBuffer, other.mBuffer);
        std::swap(mBind, other.mBind);
        std::swap(mSize, other.mSize);
        return *this;
    }

    DirectX11Buffer::operator bool() const
    {
        return mBuffer != nullptr;
    }

    void DirectX11Buffer::bindVertex(UINT stride) const
    {
        UINT offset = 0;
        sDeviceContext->IASetVertexBuffers(0, 1, &mBuffer, &stride, &offset);
        DX11_LOG("Bind Vertex Buffer -> " << mBuffer);
        DirectX11VertexArray::onBindVBO(this);
    }

    void DirectX11Buffer::bindIndex() const
    {
        sDeviceContext->IASetIndexBuffer(mBuffer, DXGI_FORMAT_R16_UINT, 0);
        DX11_LOG("Bind Index Buffer -> " << mBuffer);
        DirectX11VertexArray::onBindEBO(this);
    }

    void DirectX11Buffer::reset()
    {
        if (mBuffer) {
            mBuffer->Release();
            mBuffer = nullptr;
        }
    }

    void DirectX11Buffer::setData(const ByteBuffer &data)
    {
        if (mSize != data.mSize)
            *this = { mBind, data };
        else if (data.mSize > 0)
            sDeviceContext->UpdateSubresource(mBuffer, 0, nullptr, data.mData, data.mSize, 0);        
    }

    void DirectX11Buffer::resize(size_t size)
    {
        if (mSize != size) {
            *this = { mBind, size };
        }
    }

    WritableByteBuffer DirectX11Buffer::mapData()
    {
        struct UnmapDeleter {
            ID3D11Buffer *mBuffer;

            void operator()(void *p)
            {
                sDeviceContext->Unmap(mBuffer, 0);
            }
        };

        D3D11_MAPPED_SUBRESOURCE subres;
        ZeroMemory(&subres, sizeof(subres));

        HRESULT hr = sDeviceContext->Map(mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subres);
        DX11_CHECK(hr);

        std::unique_ptr<void, UnmapDeleter> dataBuffer { subres.pData, { mBuffer } };

        return { std::move(dataBuffer), mSize };
    }

    ID3D11Buffer *DirectX11Buffer::handle()
    {
        return mBuffer;
    }

}
}
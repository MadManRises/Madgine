#include "../directx11lib.h"

#include "directx11buffer.h"

#include "../directx11rendercontext.h"

#include "directx11vertexarray.h"

#include "Generic/bytebuffer.h"

namespace Engine {
namespace Render {

    DirectX11Buffer::DirectX11Buffer(UINT bind, const ByteBuffer &data)
        : mBind(bind)
    {
        if (data.mData) {
            D3D11_BUFFER_DESC vertexBufferDesc;
            ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

            vertexBufferDesc.BindFlags = bind;
            vertexBufferDesc.ByteWidth = data.mSize;
            vertexBufferDesc.CPUAccessFlags = 0;
            vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

            D3D11_SUBRESOURCE_DATA resourceData;
            ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));

            resourceData.pSysMem = data.mData;

            HRESULT hr = sDevice->CreateBuffer(&vertexBufferDesc, &resourceData, &mBuffer);
            DX11_CHECK(hr);
        }
    }

    DirectX11Buffer::DirectX11Buffer(DirectX11Buffer &&other)
        : mBind(std::exchange(other.mBind, 0))
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
        *this = { mBind, data };
    }

    ID3D11Buffer *DirectX11Buffer::handle()
    {
        return mBuffer;
    }

}
}
#include "../directx11lib.h"

#include "directx11buffer.h"

#include "../directx11rendercontext.h"

#include "directx11vertexarray.h"

namespace Engine {
namespace Render {

    DirectX11Buffer::DirectX11Buffer(UINT bind, size_t size, const void *data)
        : mBind(bind)
    {
        if (data) {
            D3D11_BUFFER_DESC vertexBufferDesc;
            ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

            vertexBufferDesc.BindFlags = bind;
            vertexBufferDesc.ByteWidth = size;
            vertexBufferDesc.CPUAccessFlags = 0;
            vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

            D3D11_SUBRESOURCE_DATA resourceData;
            ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));

            resourceData.pSysMem = data;

            HRESULT hr = sDevice->CreateBuffer(&vertexBufferDesc, &resourceData, &mBuffer);
            DX11_CHECK(hr);
        }
    }

    DirectX11Buffer::DirectX11Buffer(DirectX11Buffer &&other)
        : mBuffer(std::exchange(other.mBuffer, nullptr))
        , mBind(std::exchange(other.mBind, 0))
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

    void DirectX11Buffer::setData(size_t size, const void *data)
    {
        *this = { mBind, size, data };
    }

    ID3D11Buffer *DirectX11Buffer::handle()
    {
        return mBuffer;
    }

}
}
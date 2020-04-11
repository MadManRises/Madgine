#include "../directx11lib.h"

#include "directx11texture.h"

#include "../directx11rendercontext.h"

namespace Engine {
namespace Render {

    DirectX11Texture::DirectX11Texture(TextureType type, DataFormat format, UINT bind, size_t width, size_t height, const ByteBuffer &data)
        : mType(type)
        , mSize({ static_cast<int>(width), static_cast<int>(height) })
        , mFormat(format)
        , mBind(bind)
    {
        DXGI_FORMAT xFormat;
        D3D11_SRV_DIMENSION dimension;
        size_t byteCount;
        switch (format) {
        case FORMAT_UNSIGNED_BYTE:
            xFormat = DXGI_FORMAT_R8G8B8A8_UINT;
            byteCount = 4;
            break;
        case FORMAT_FLOAT8:
            xFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
            byteCount = 4;
            break;
        case FORMAT_FLOAT32:
            xFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
            byteCount = 16;
            break;
        default:
            std::terminate();
        }

        switch (type) {
        case Texture2D: {
            D3D11_TEXTURE2D_DESC textureDesc;
            ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

            textureDesc.ArraySize = 1;
            textureDesc.BindFlags = bind;
            textureDesc.CPUAccessFlags = 0;
            textureDesc.Format = xFormat;
            textureDesc.Width = width;
            textureDesc.Height = height;
            textureDesc.MipLevels = 1;
            textureDesc.SampleDesc.Count = 1;
            textureDesc.SampleDesc.Quality = 0;
            textureDesc.Usage = D3D11_USAGE_DEFAULT;

            D3D11_SUBRESOURCE_DATA subResourceDesc;
            subResourceDesc.pSysMem = data.mData;
            subResourceDesc.SysMemPitch = width * byteCount;
            subResourceDesc.SysMemSlicePitch = 0;

            ID3D11Texture2D *tex;
            HRESULT hr = sDevice->CreateTexture2D(&textureDesc, data.mData ? &subResourceDesc : nullptr, &tex);
            mResource = tex;
            DX11_CHECK(hr);

            dimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            break;
        }
        default:
            std::terminate();
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
        shaderResourceViewDesc.Format = xFormat;
        shaderResourceViewDesc.ViewDimension = dimension;
        shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
        shaderResourceViewDesc.Texture2D.MipLevels = 1;

        HRESULT hr = sDevice->CreateShaderResourceView(mResource, &shaderResourceViewDesc, reinterpret_cast<ID3D11ShaderResourceView **>(&mTextureHandle));
        DX11_CHECK(hr);

        D3D11_SAMPLER_DESC samplerDesc;

        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        // Create the texture sampler state.
        hr = sDevice->CreateSamplerState(&samplerDesc, &mSampler);
        DX11_CHECK(hr);
    }

    DirectX11Texture::DirectX11Texture(TextureType type, DataFormat format, UINT bind)
        : mType(type)
        , mFormat(format)
        , mBind(bind)
    {
    }

    DirectX11Texture::DirectX11Texture(DirectX11Texture &&other)
        : mResource(std::exchange(other.mResource, nullptr))
        , mType(other.mType)
        , mFormat(std::exchange(other.mFormat, {}))
        , mBind(std::exchange(other.mBind, 0))
    {
        mTextureHandle = std::exchange(other.mTextureHandle, 0);
    }

    DirectX11Texture::~DirectX11Texture()
    {
        reset();
    }

    DirectX11Texture &DirectX11Texture::operator=(DirectX11Texture &&other)
    {
        std::swap(mResource, other.mResource);
        std::swap(mTextureHandle, other.mTextureHandle);
        std::swap(mType, other.mType);
        std::swap(mBind, other.mBind);
        std::swap(mFormat, other.mFormat);
        return *this;
    }

    void DirectX11Texture::reset()
    {
        if (mTextureHandle) {
            switch (mType) {
            case Texture2D:
                static_cast<ID3D11Texture2D *>(mResource)->Release();
                break;
            default:
                std::terminate();
            }
            mResource = nullptr;
            reinterpret_cast<ID3D11ShaderResourceView *>(mTextureHandle)->Release();
            mTextureHandle = 0;
        }
    }

    void DirectX11Texture::bind() const
    {
        sDeviceContext->PSSetShaderResources(0, 1, reinterpret_cast<ID3D11ShaderResourceView *const *>(&mTextureHandle));
        sDeviceContext->PSSetSamplers(0, 1, &mSampler);
    }

    void DirectX11Texture::setData(Vector2i size, const ByteBuffer &data)
    {
        *this = DirectX11Texture { mType, mFormat, mBind, static_cast<size_t>(size.x), static_cast<size_t>(size.y), data };
    }

    void DirectX11Texture::setSubData(Vector2i offset, Vector2i size, const ByteBuffer &data)
    {
        size_t byteCount;
        switch (mFormat) {
        case FORMAT_UNSIGNED_BYTE:
            byteCount = 4;
            break;
        case FORMAT_FLOAT8:
            byteCount = 4;
            break;
        case FORMAT_FLOAT32:
            byteCount = 16;
            break;
        default:
            std::terminate();
        }

        D3D11_BOX box;
        box.top = offset.y;
        box.left = offset.x;
        box.right = offset.x + size.x;
        box.bottom = offset.y + size.y;
        box.front = 0;
        box.back = 1;

        sDeviceContext->UpdateSubresource(mResource, 0, &box, data.mData, size.x * byteCount, 0);
    }

    void DirectX11Texture::resize(Vector2i size)
    {
        throw "TODO";
        /*Vector2i commonSize = min(size, mSize);
        GLuint tempTex;
        glGenTextures(1, &tempTex);
        GL_CHECK();
        std::swap(tempTex, mHandle);

        setData(size, nullptr);

        glCopyImageSubData(tempTex, GL_TEXTURE_2D, 0, 0, 0, 0, mHandle, GL_TEXTURE_2D, 0, 0, 0, 0, commonSize.x, commonSize.y, 1);
        GL_CHECK();

        glDeleteTextures(1, &tempTex);
        GL_CHECK();*/
    }

    TextureHandle DirectX11Texture::handle() const
    {
        return mTextureHandle;
    }

    ID3D11Resource *DirectX11Texture::resource() const
    {
        return mResource;
    }

    /*void DirectX11Texture::setWrapMode(GLint mode)
    {
        bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
        GL_CHECK();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
        GL_CHECK();
    }

    void DirectX11Texture::setFilter(GLint filter)
    {
        bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        GL_CHECK();
    }*/
}
}
#include "../directx11lib.h"

#include "directx11texture.h"

#include "../directx11rendercontext.h"

namespace Engine {
namespace Render {

    DirectX11Texture::DirectX11Texture(TextureType type, DataFormat format, UINT bind, size_t width, size_t height, size_t samples, const ByteBuffer &data)
        : Texture(type, format, { static_cast<int>(width), static_cast<int>(height) })
        , mBind(bind)
        , mSamples(samples)
    {
        DXGI_FORMAT xFormat, xViewFormat;
        size_t byteCount;
        switch (format) {
        case FORMAT_RGBA8:
            xFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
            xViewFormat = xFormat;
            byteCount = 4;
            break;
        case FORMAT_RGBA8_SRGB:
            xFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            xViewFormat = xFormat;
            byteCount = 4;
            break;
        case FORMAT_RGBA16F:
            xFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
            xViewFormat = xFormat;
            byteCount = 8;
            break;
        case FORMAT_D24:
            xFormat = DXGI_FORMAT_R24G8_TYPELESS;
            xViewFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
            byteCount = 0;
            assert(!data.mData);
            break;
        case FORMAT_D32:
            xFormat = DXGI_FORMAT_R32_TYPELESS;
            xViewFormat = DXGI_FORMAT_R32_FLOAT;
            byteCount = 0;
            assert(!data.mData);
            break;
        default:
            std::terminate();
        }

        D3D11_TEXTURE2D_DESC textureDesc;
        ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

        textureDesc.ArraySize = 1;
        textureDesc.BindFlags = bind;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.Format = xFormat;
        textureDesc.Width = width;
        textureDesc.Height = height;
        textureDesc.MipLevels = 1;
        textureDesc.SampleDesc.Count = samples;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;

        if (type == TextureType_Cube) {
            textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
            textureDesc.ArraySize = 6;
        }

        D3D11_SUBRESOURCE_DATA subResourceDesc;
        subResourceDesc.pSysMem = data.mData;
        subResourceDesc.SysMemPitch = width * byteCount;
        subResourceDesc.SysMemSlicePitch = 0;

        ReleasePtr<ID3D11Texture2D> tex;
        HRESULT hr = sDevice->CreateTexture2D(&textureDesc, data.mData ? &subResourceDesc : nullptr, &tex);
        mResource = std::move(tex);
        DX11_CHECK(hr);

        D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
        shaderResourceViewDesc.Format = xViewFormat;
        shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
        shaderResourceViewDesc.Texture2D.MipLevels = 1;

        assert(samples == 1 || type == TextureType_2DMultiSample);

        switch (type) {
        case TextureType_2D:
            shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            break;
        case TextureType_2DMultiSample:
            shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
            break;
        case TextureType_Cube:
            shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
            shaderResourceViewDesc.Texture2DArray.ArraySize = 6;
            shaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
            break;
        default:
            throw 0;
        }

        hr = sDevice->CreateShaderResourceView(mResource, &shaderResourceViewDesc, reinterpret_cast<ID3D11ShaderResourceView **>(&mTextureHandle));
        DX11_CHECK(hr);

        DX11_LOG("Created SRV: " << reinterpret_cast<ID3D11ShaderResourceView *>(mTextureHandle));
    }

    DirectX11Texture::DirectX11Texture(TextureType type, DataFormat format, UINT bind, size_t samples)
        : Texture(type, format)
        , mBind(bind)
        , mSamples(samples)
    {
    }

    DirectX11Texture::DirectX11Texture(DirectX11Texture &&other)
        : Texture(std::move(other))
        , mResource(std::exchange(other.mResource, nullptr))
        , mBind(std::exchange(other.mBind, 0))
        , mSamples(std::exchange(other.mSamples, 1))
    {
    }

    DirectX11Texture::~DirectX11Texture()
    {
        reset();
    }

    DirectX11Texture &DirectX11Texture::operator=(DirectX11Texture &&other)
    {
        Texture::operator=(std::move(other));
        std::swap(mResource, other.mResource);
        std::swap(mBind, other.mBind);
        std::swap(mSamples, other.mSamples);
        return *this;
    }

    void DirectX11Texture::reset()
    {
        mResource.reset();
        if (mTextureHandle) {
            DX11_LOG("Destroying SRV: " << reinterpret_cast<ID3D11ShaderResourceView *>(mTextureHandle));
            reinterpret_cast<ID3D11ShaderResourceView *>(mTextureHandle)->Release();
            mTextureHandle = 0;
        }
    }

    void DirectX11Texture::setData(Vector2i size, const ByteBuffer &data)
    {
        *this = DirectX11Texture { mType, mFormat, mBind, static_cast<size_t>(size.x), static_cast<size_t>(size.y), mSamples, data };
    }

    void DirectX11Texture::setSubData(Vector2i offset, Vector2i size, const ByteBuffer &data)
    {
        size_t byteCount;
        switch (mFormat) {
        case FORMAT_RGBA8:
            byteCount = 4;
            break;
        case FORMAT_RGBA8_SRGB:
            byteCount = 4;
            break;
        case FORMAT_RGBA16F:
            byteCount = 8;
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
        setData(size, {});
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

    ID3D11Resource *DirectX11Texture::resource() const
    {
        return mResource;
    }

}
}
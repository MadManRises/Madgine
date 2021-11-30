#include "directx11lib.h"

#include "directx11rendertexture.h"

#include "Meta/math/matrix4.h"
#include "Meta/math/vector4.h"

#include "directx11renderwindow.h"

#include "Madgine/render/camera.h"

#include "Madgine/render/renderpass.h"
#include "directx11meshdata.h"
#include "directx11meshloader.h"

#include "directx11rendercontext.h"

namespace Engine {
namespace Render {

    DirectX11RenderTexture::DirectX11RenderTexture(DirectX11RenderContext *context, const Vector2i &size, const RenderTextureConfig &config)
        : DirectX11RenderTarget(context, false, config.mName, config.mIterations)
        , mType(config.mType)
        , mSamples(config.mSamples)
        , mSize { 0, 0 }
    {
        size_t bufferCount = config.mIterations > 1 && config.mSamples == 1 ? 2 : 1;

        for (size_t i = 0; i < config.mTextureCount * bufferCount; ++i) {
            DirectX11Texture &tex = mTextures.emplace_back(config.mType, config.mFormat, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
        }

        if (mSamples > 1) {
            for (size_t i = 0; i < config.mTextureCount; ++i) {
                mMultisampledTextures.emplace_back(TextureType_2DMultiSample, config.mFormat, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, mSamples);
            }
        }

        resize(size);
    }

    DirectX11RenderTexture::~DirectX11RenderTexture()
    {
        shutdown();
    }

    bool DirectX11RenderTexture::resizeImpl(const Vector2i &size)
    {
        if (mSize == size)
            return false;

        mSize = size;

        std::vector<ReleasePtr<ID3D11RenderTargetView>> targetViews;

        if (mSamples > 1) {
            for (DirectX11Texture &tex : mTextures)
                tex.resize(size);
        }

        for (DirectX11Texture &tex : (mSamples > 1 ? mMultisampledTextures : mTextures)) {
            tex.resize(size);

            D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

            switch (tex.format()) {
            case FORMAT_RGBA8:
                renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                break;
            case FORMAT_RGBA16F:
                renderTargetViewDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
                break;
            default:
                throw 0;
            }
            switch (tex.type()) {
            case TextureType_2D:
                renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                break;
            case TextureType_2DMultiSample:
                renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
            }
            renderTargetViewDesc.Texture2D.MipSlice = 0;

            HRESULT hr = sDevice->CreateRenderTargetView(tex.resource(), &renderTargetViewDesc, &targetViews.emplace_back());
            DX11_CHECK(hr);
        }

        setup(std::move(targetViews), size, mType, mSamples);

        return true;
    }

    void DirectX11RenderTexture::beginIteration(size_t iteration) const
    {
        DirectX11RenderTarget::beginIteration(iteration);

        if (iteration > 0)
            bindTextures({ mTextures[1 - (iteration % 2)].descriptor() });
    }

    void DirectX11RenderTexture::endIteration(size_t iteration) const
    {
        DirectX11RenderTarget::endIteration(iteration);

        if (mSamples > 1) {
            for (size_t i = 0; i < mTextures.size(); ++i) {
                DXGI_FORMAT xFormat;
                switch (mTextures[i].format()) {
                case FORMAT_RGBA8:
                    xFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
                    break;
                case FORMAT_RGBA16F:
                    xFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
                    break;
                case FORMAT_D24:
                    xFormat = DXGI_FORMAT_R24G8_TYPELESS;
                    break;
                case FORMAT_D32:
                    xFormat = DXGI_FORMAT_R32_TYPELESS;
                    break;
                default:
                    std::terminate();
                }

                sDeviceContext->ResolveSubresource(mTextures[i].resource(), 0, mMultisampledTextures[i].resource(), 0, xFormat);
            }
        }
    }

    TextureDescriptor DirectX11RenderTexture::texture(size_t index, size_t iteration) const
    {
        if (iteration == std::numeric_limits<size_t>::max())
            iteration = iterations();

        int bufferCount = iterations() > 1 ? 2 : 1;
        int offset = iterations() > 1 ? 1 - iteration % 2 : 0;
        return mTextures[(mTextures.size() / bufferCount) * offset + index].descriptor();
    }

    size_t DirectX11RenderTexture::textureCount() const
    {
        return mTextures.size();
    }

    TextureDescriptor DirectX11RenderTexture::depthTexture() const
    {
        return mDepthBuffer.descriptor();
    }

    Vector2i DirectX11RenderTexture::size() const
    {
        return mSize;
    }

}
}

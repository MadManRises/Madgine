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

#include "util/directx11pipelineinstance.h"

namespace Engine {
namespace Render {

    DirectX11RenderTexture::DirectX11RenderTexture(DirectX11RenderContext *context, const Vector2i &size, const RenderTextureConfig &config)
        : DirectX11RenderTarget(context, false, config.mName, config.mFlipFlop, config.mBlitSource)
        , mType(config.mType)
        , mSamples(config.mSamples)
        , mSize { 0, 0 }
    {
        size_t bufferCount = config.mFlipFlop ? 2 : 1;

        for (size_t i = 0; i < config.mTextureCount * bufferCount; ++i) {
            DirectX11Texture &tex = mTextures.emplace_back(config.mType, config.mFormat, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, mSamples);
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

        size_t count = 1;
        std::vector<std::array<ReleasePtr<ID3D11RenderTargetView>, 6>> targetViews;

        for (DirectX11Texture &tex : mTextures) {
            tex.resize(size);

            D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

            switch (tex.format()) {
            case FORMAT_RGBA8:
                renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                break;
            case FORMAT_RGBA8_SRGB:
                renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
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
                break;
            default:
                throw 0;
            }
            renderTargetViewDesc.Texture2D.MipSlice = 0;

            std::array<ReleasePtr<ID3D11RenderTargetView>, 6> &views = targetViews.emplace_back();
            for (size_t i = 0; i < count; ++i) {
                HRESULT hr = sDevice->CreateRenderTargetView(tex.resource(), &renderTargetViewDesc, &views[i]);
                DX11_CHECK(hr);
            }
        }

        setup(std::move(targetViews), size, mType, mSamples);

        return true;
    }

    void DirectX11RenderTexture::beginIteration(bool flipFlopping, size_t targetIndex, size_t targetCount, size_t targetSubresourceIndex) const
    {
        DirectX11RenderTarget::beginIteration(flipFlopping, targetIndex, targetCount, targetSubresourceIndex);
    }

    void DirectX11RenderTexture::endIteration() const
    {
        DirectX11RenderTarget::endIteration();
    }

    void DirectX11RenderTexture::beginFrame()
    {
        DirectX11RenderTarget::beginFrame();

        if (mBlitSource)
            blit(mBlitSource);
    }

    const DirectX11Texture &DirectX11RenderTexture::getTexture(size_t index) const
    {
        int offset = canFlipFlop() ? mFlipFlopIndices[index] : 0;
        return mTextures[textureCount() * offset + index];
    }

    TextureDescriptor DirectX11RenderTexture::texture(size_t index) const
    {
        return getTexture(index).descriptor();
    }

    size_t DirectX11RenderTexture::textureCount() const
    {
        int bufferCount = canFlipFlop() ? 2 : 1;
        return mTextures.size() / bufferCount;
    }

    TextureDescriptor DirectX11RenderTexture::depthTexture() const
    {
        return mDepthBuffer.descriptor();
    }

    void DirectX11RenderTexture::blit(RenderTarget *input) const
    {
        DirectX11RenderTexture *inputTex = dynamic_cast<DirectX11RenderTexture *>(input);
        assert(inputTex);

        size_t count = std::min(mTextures.size(), inputTex->mTextures.size());

        int bufferCount = canFlipFlop() ? 2 : 1;

        for (size_t i = 0; i < count; ++i) {
            DXGI_FORMAT xFormat;
            switch (mTextures[i].format()) {
            case FORMAT_RGBA8:
                xFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
                break;
            case FORMAT_RGBA8_SRGB:
                xFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
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

            sDeviceContext->ResolveSubresource(getTexture(i).resource(), 0, inputTex->getTexture(i).resource(), 0, xFormat);
        }
    }

    Vector2i DirectX11RenderTexture::size() const
    {
        return mSize;
    }

}
}

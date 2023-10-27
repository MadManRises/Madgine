#include "directx12lib.h"

#include "directx12rendertexture.h"

#include "directx12rendercontext.h"

namespace Engine {
namespace Render {

    DirectX12RenderTexture::DirectX12RenderTexture(DirectX12RenderContext *context, const Vector2i &size, const RenderTextureConfig &config)
        : DirectX12RenderTarget(context, false, config.mName, config.mBlitSource)
        , mSize { 0, 0 }
        , mSamples(config.mSamples)
    {
        size_t bufferCount = config.mFlipFlop ? 2 : 1;

        for (size_t i = 0; i < config.mTextureCount * bufferCount; ++i) {
            mTextures.emplace_back(config.mType, true, config.mFormat, config.mSamples);
        }

        if (config.mCreateDepthBufferView)
            mDepthBufferView = DirectX12RenderContext::getSingleton().mDescriptorHeap.allocate();

        mTargetViews.resize(mTextures.size());
        for (OffsetPtr &ptr : mTargetViews)
            ptr = DirectX12RenderContext::getSingleton().mRenderTargetDescriptorHeap.allocate();

        resize(size);

        for (DirectX12Texture &tex : mTextures) {
            tex.setName(config.mName.empty() ? "RenderTexture" : config.mName);
        }
    }

    DirectX12RenderTexture::~DirectX12RenderTexture()
    {
        shutdown();
    }

    bool DirectX12RenderTexture::resizeImpl(const Vector2i &size)
    {
        if (mSize == size)
            return false;

        mSize = size;

        size_t i = 0;
        std::vector<OffsetPtr> targetViews = std::move(mTargetViews);

        for (DirectX12Texture &tex : mTextures) {
            tex.setData(size, {});

            D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDesc {};

            switch (tex.format()) {
            case FORMAT_RGBA8:
                renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                break;
            case FORMAT_RGBA8_SRGB:
                renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
                break;
            default:
                throw 0;
            }

            renderTargetViewDesc.ViewDimension = mSamples > 1 ? D3D12_RTV_DIMENSION_TEXTURE2DMS : D3D12_RTV_DIMENSION_TEXTURE2D;
            renderTargetViewDesc.Texture2D.MipSlice = 0;

            GetDevice()->CreateRenderTargetView(tex, &renderTargetViewDesc, DirectX12RenderContext::getSingleton().mRenderTargetDescriptorHeap.cpuHandle(targetViews[i]));

            ++i;
        }

        setup(std::move(targetViews), size, mSamples, bool(mDepthBufferView));

        if (mDepthBufferView) {
            D3D12_SHADER_RESOURCE_VIEW_DESC depthViewDesc;
            ZeroMemory(&depthViewDesc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
            depthViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            depthViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
            depthViewDesc.ViewDimension = mSamples > 1 ? D3D12_SRV_DIMENSION_TEXTURE2DMS : D3D12_SRV_DIMENSION_TEXTURE2D;
            depthViewDesc.Texture2D.MipLevels = 1;

            GetDevice()->CreateShaderResourceView(mDepthStencilBuffer, &depthViewDesc, DirectX12RenderContext::getSingleton().mDescriptorHeap.cpuHandle(mDepthBufferView));
        }

        DX12_CHECK();

        return true;
    }

    void DirectX12RenderTexture::beginIteration(bool flipFlopping, size_t targetIndex, size_t targetCount, size_t targetSubresourceIndex) const
    {
        for (const DirectX12Texture &tex : mTextures)
            mCommandList.Transition(tex, tex.readStateFlags(), mBlitSource ? D3D12_RESOURCE_STATE_RESOLVE_DEST : D3D12_RESOURCE_STATE_RENDER_TARGET);

        if (mDepthBufferView)
            mCommandList.Transition(mDepthStencilBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);

        DirectX12RenderTarget::beginIteration(flipFlopping, targetIndex, targetCount, targetSubresourceIndex);

        if (mBlitSource)
            blit(mBlitSource);
    }

    void DirectX12RenderTexture::endIteration() const
    {
        DirectX12RenderTarget::endIteration();

        for (const DirectX12Texture &tex : mTextures)
            mCommandList.Transition(tex, mBlitSource ? D3D12_RESOURCE_STATE_RESOLVE_DEST : D3D12_RESOURCE_STATE_RENDER_TARGET, tex.readStateFlags());

        if (mDepthBufferView)
            mCommandList.Transition(mDepthStencilBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    }

    TextureDescriptor DirectX12RenderTexture::texture(size_t index) const
    {
        int bufferCount = canFlipFlop() ? 2 : 1;
        int offset = canFlipFlop() ? mFlipFlopIndices[index] : 0;
        return mTextures[(mTextures.size() / bufferCount) * offset + index].descriptor();
    }

    size_t DirectX12RenderTexture::textureCount() const
    {
        return 1;
    }

    TextureDescriptor DirectX12RenderTexture::depthTexture() const
    {
        return { DirectX12RenderContext::getSingleton().mDescriptorHeap.gpuHandle(mDepthBufferView).ptr, TextureType_2D };
    }

    const std::vector<DirectX12Texture> &DirectX12RenderTexture::textures() const
    {
        return mTextures;
    }

    Vector2i DirectX12RenderTexture::size() const
    {
        return mSize;
    }

    void DirectX12RenderTexture::beginFrame()
    {
        DirectX12RenderTarget::beginFrame();
        for (DirectX12Texture &tex : mTextures)
            mCommandList.attachResource(tex.operator ReleasePtr<ID3D12Resource>());
    }

    void DirectX12RenderTexture::endFrame()
    {
        DirectX12RenderTarget::endFrame();
    }

    void DirectX12RenderTexture::blit(RenderTarget *input) const
    {
        DirectX12RenderTexture *inputTex = dynamic_cast<DirectX12RenderTexture *>(input);
        assert(inputTex);

        size_t count = std::min(mTextures.size(), inputTex->mTextures.size());

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

            mCommandList->ResolveSubresource(mTextures[i], 0, inputTex->mTextures[i], 0, xFormat);
        }
    }

}
}

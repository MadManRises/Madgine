#include "directx12lib.h"

#include "directx12rendertexture.h"

#include "directx12rendercontext.h"

namespace Engine {
namespace Render {

    DirectX12RenderTexture::DirectX12RenderTexture(DirectX12RenderContext *context, const Vector2i &size, const RenderTextureConfig &config)
        : DirectX12RenderTarget(context, false, config.mName)
        , mTexture(TextureType_2D, true, FORMAT_RGBA8)
        , mSize { 0, 0 }
    {
        //context->waitForGPU();

        resize(size, config);

        mTexture.setName(config.mName.empty() ? "RenderTexture" : config.mName);
    }

    DirectX12RenderTexture::~DirectX12RenderTexture()
    {
        shutdown();
    }

    bool DirectX12RenderTexture::resize(const Vector2i &size, const RenderTextureConfig &config)
    {
        if (mSize == size)
            return false;

        mSize = size;

        //mTexture.resize(size);
        mTexture.setData(size, {});

        D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDesc {};

        renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        renderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        renderTargetViewDesc.Texture2D.MipSlice = 0;

        OffsetPtr targetView = DirectX12RenderContext::getSingleton().mRenderTargetDescriptorHeap.allocate();
        GetDevice()->CreateRenderTargetView(mTexture.resource(), &renderTargetViewDesc, DirectX12RenderContext::getSingleton().mRenderTargetDescriptorHeap.cpuHandle(targetView));

        setup(targetView, size);

        if (mDepthBufferView || config.mCreateDepthBufferView) {
            if (mDepthBufferView) {
                throw 0;
                /*mDepthBufferView->Release();
                mDepthBufferView = nullptr;*/
            }
            D3D12_SHADER_RESOURCE_VIEW_DESC depthViewDesc;
            ZeroMemory(&depthViewDesc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
            depthViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            depthViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
            depthViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            depthViewDesc.Texture2D.MipLevels = 1; 

            mDepthBufferView = DirectX12RenderContext::getSingleton().mDescriptorHeap.allocate();
            GetDevice()->CreateShaderResourceView(mDepthStencilBuffer, &depthViewDesc, DirectX12RenderContext::getSingleton().mDescriptorHeap.cpuHandle(mDepthBufferView));            
        }

        DX12_CHECK();

        return true;
    }

    bool DirectX12RenderTexture::resizeImpl(const Vector2i &size)
    {
        return resize(size, {});
    }

    void DirectX12RenderTexture::beginIteration(size_t iteration) const
    {
        Transition(mTexture.resource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

        DirectX12RenderTarget::beginIteration(iteration);
    }

    void DirectX12RenderTexture::endIteration(size_t iteration) const
    {
        Transition(mTexture.resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        DirectX12RenderTarget::endIteration(iteration);
    }

    TextureDescriptor DirectX12RenderTexture::texture(size_t index, size_t iteration) const
    {
        return mTexture.descriptor();
    }

    size_t DirectX12RenderTexture::textureCount() const
    {
        return 1;
    }

    TextureDescriptor DirectX12RenderTexture::depthTexture() const
    {
        return { DirectX12RenderContext::getSingleton().mDescriptorHeap.gpuHandle(mDepthBufferView).ptr, TextureType_2D };
    }

    Vector2i DirectX12RenderTexture::size() const
    {
        return mSize;
    }

}
}

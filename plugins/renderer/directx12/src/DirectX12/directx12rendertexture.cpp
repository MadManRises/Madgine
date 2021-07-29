#include "directx12lib.h"

#include "directx12rendertexture.h"

#include "Meta/math/matrix4.h"
#include "Meta/math/vector4.h"

#include "directx12renderwindow.h"

#include "Madgine/render/camera.h"

#include "Madgine/render/renderpass.h"
#include "directx12meshdata.h"
#include "directx12meshloader.h"

#include "directx12rendercontext.h"

namespace Engine {
namespace Render {

    DirectX12RenderTexture::DirectX12RenderTexture(DirectX12RenderContext *context, const Vector2i &size, const RenderTextureConfig &config)
        : DirectX12RenderTarget(context)
        , mTexture(RenderTarget2D, FORMAT_FLOAT8)
        , mSize { 0, 0 }
    {
        //context->waitForGPU();

        resize(size, config);
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

        D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
        ZeroMemory(&renderTargetViewDesc, sizeof(D3D12_RENDER_TARGET_VIEW_DESC));

        renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        renderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        renderTargetViewDesc.Texture2D.MipSlice = 0;

        OffsetPtr targetView = DirectX12RenderContext::getSingleton().mRenderTargetDescriptorHeap.allocate();
        sDevice->CreateRenderTargetView(mTexture.resource(), &renderTargetViewDesc, DirectX12RenderContext::getSingleton().mRenderTargetDescriptorHeap.cpuHandle(targetView));

        setup(targetView, size, config);

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
            sDevice->CreateShaderResourceView(mDepthStencilBuffer, &depthViewDesc, DirectX12RenderContext::getSingleton().mDescriptorHeap.cpuHandle(mDepthBufferView));            
        }

        DX12_CHECK();

        return true;
    }

    bool DirectX12RenderTexture::resize(const Vector2i &size)
    {
        return resize(size, {});
    }

    void DirectX12RenderTexture::beginFrame()
    {
        DirectX12RenderTarget::beginFrame();
    }

    void DirectX12RenderTexture::endFrame()
    {
        DirectX12RenderTarget::endFrame();
    }

    const DirectX12Texture *DirectX12RenderTexture::texture() const
    {
        return &mTexture;
    }

    TextureHandle DirectX12RenderTexture::depthTexture() const
    {
        return mDepthBufferView.offset();
    }

    Vector2i DirectX12RenderTexture::size() const
    {
        return mSize;
    }

}
}
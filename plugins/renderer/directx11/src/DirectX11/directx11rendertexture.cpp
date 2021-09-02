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
        : DirectX11RenderTarget(context)
        , mTexture(Texture2D, FORMAT_FLOAT32, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
        , mSize { 0, 0 }
    {
        resize(size, config);
    }

    DirectX11RenderTexture::~DirectX11RenderTexture()
    {
        shutdown();
    }

    bool DirectX11RenderTexture::resize(const Vector2i &size, const RenderTextureConfig &config)
    {
        if (mSize == size)
            return false;

        mSize = size;

        //mTexture.resize(size);
        mTexture.setData(size, {});

        D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

        renderTargetViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        renderTargetViewDesc.Texture2D.MipSlice = 0;

        ID3D11RenderTargetView *targetView;

        HRESULT hr = sDevice->CreateRenderTargetView(mTexture.resource(), &renderTargetViewDesc, &targetView);
        DX11_CHECK(hr);

        setup(targetView, size, config);

        if (mDepthBufferView || config.mCreateDepthBufferView) {
            if (mDepthBufferView) {
                mDepthBufferView->Release();
                mDepthBufferView = nullptr;
            }
            D3D11_SHADER_RESOURCE_VIEW_DESC depthViewDesc;
            ZeroMemory(&depthViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
            depthViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
            depthViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            depthViewDesc.Texture2D.MipLevels = 1; 

            hr = sDevice->CreateShaderResourceView(mDepthStencilBuffer, &depthViewDesc, &mDepthBufferView);
            DX11_CHECK(hr);
        }

        return true;
    }

    bool DirectX11RenderTexture::resize(const Vector2i &size)
    {
        return resize(size, {});
    }

    void DirectX11RenderTexture::beginFrame()
    {
        DirectX11RenderTarget::beginFrame();
    }

    void DirectX11RenderTexture::endFrame()
    {
        DirectX11RenderTarget::endFrame();
    }

    TextureHandle DirectX11RenderTexture::texture() const
    {
        return mTexture.mTextureHandle;
    }

    TextureHandle DirectX11RenderTexture::depthTexture() const
    {
        return reinterpret_cast<TextureHandle>(mDepthBufferView);
    }

    Vector2i DirectX11RenderTexture::size() const
    {
        return mSize;
    }

}
}

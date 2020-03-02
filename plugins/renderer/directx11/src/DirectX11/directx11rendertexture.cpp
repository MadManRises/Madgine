#include "directx11lib.h"

#include "directx11rendertexture.h"

#include "Modules/math/matrix4.h"
#include "Modules/math/vector4.h"

#include "directx11renderwindow.h"

#include "Madgine/render/camera.h"

#include "Madgine/render/renderpass.h"
#include "directx11meshdata.h"
#include "directx11meshloader.h"

#include "directx11rendercontext.h"

namespace Engine {
namespace Render {

    DirectX11RenderTexture::DirectX11RenderTexture(DirectX11RenderContext *context, const Vector2i &size)
        : DirectX11RenderTarget(context)
        , mTexture(Texture2D, FORMAT_FLOAT32, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
        , mSize({ 0, 0 })
    {
        resize(size);
    }

    DirectX11RenderTexture::~DirectX11RenderTexture()
    {
        shutdown();
    }

    bool DirectX11RenderTexture::resize(const Vector2i &size)
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

        setup(targetView, size);

        return true;
    }

    void DirectX11RenderTexture::beginFrame()
    {
        DirectX11RenderTarget::beginFrame();
    }

    void DirectX11RenderTexture::endFrame()
    {
        DirectX11RenderTarget::endFrame();
    }

    const DirectX11Texture *DirectX11RenderTexture::texture() const
    {
        return &mTexture;
    }

    Vector2i DirectX11RenderTexture::size() const
    {
        return mSize;
    }

}
}

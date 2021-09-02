#pragma once

#include "directx12rendertarget.h"
#include "Meta/math/matrix4.h"
#include "util/directx12program.h"
#include "util/directx12buffer.h"
#include "util/directx12texture.h"

#include "render/renderpassflags.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12RenderTexture : DirectX12RenderTarget {

        DirectX12RenderTexture(DirectX12RenderContext *context, const Vector2i &size, const RenderTextureConfig &config);
        ~DirectX12RenderTexture();
        
        bool resize(const Vector2i &size, const RenderTextureConfig &config);
        bool resize(const Vector2i &size) override;
        Vector2i size() const override;
        
        virtual void beginFrame() override;
        virtual void endFrame() override;

        virtual TextureHandle texture() const override;        
        virtual TextureHandle depthTexture() const override;
        
    private:        
        OffsetPtr mDepthBufferView;
        DirectX12Texture mTexture;

		Vector2i mSize;
    };

}
}
#pragma once

#include "directx12rendertarget.h"
#include "util/directx12texture.h"


namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12RenderTexture : DirectX12RenderTarget {

        DirectX12RenderTexture(DirectX12RenderContext *context, const Vector2i &size, const RenderTextureConfig &config);
        ~DirectX12RenderTexture();
        
        bool resize(const Vector2i &size, const RenderTextureConfig &config);
        bool resizeImpl(const Vector2i &size) override;
        Vector2i size() const override;
        
        virtual void beginIteration(size_t iteration) override;
        virtual void endIteration(size_t iteration) override;

        virtual TextureDescriptor texture(size_t index, size_t iteration = std::numeric_limits<size_t>::max()) const override;        
        virtual size_t textureCount() const override;
        virtual TextureDescriptor depthTexture() const override;
        
    private:        
        OffsetPtr mDepthBufferView;
        DirectX12Texture mTexture;

		Vector2i mSize;
    };

}
}
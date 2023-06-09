#pragma once

#include "directx12rendertarget.h"
#include "util/directx12texture.h"


namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12RenderTexture : DirectX12RenderTarget {

        DirectX12RenderTexture(DirectX12RenderContext *context, const Vector2i &size, const RenderTextureConfig &config);
        ~DirectX12RenderTexture();
        
        bool resizeImpl(const Vector2i &size) override;
        Vector2i size() const override;
        
        virtual void beginFrame() override;
        virtual void endFrame() override;

        virtual void beginIteration(size_t iteration) const override;
        virtual void endIteration(size_t iteration) const override;

        virtual TextureDescriptor texture(size_t index, size_t iteration = std::numeric_limits<size_t>::max()) const override;        
        virtual size_t textureCount() const override;
        virtual TextureDescriptor depthTexture() const override;

        void blit(RenderTarget *input) const;
        
        const std::vector<DirectX12Texture> &textures() const;

    private:        
        OffsetPtr mDepthBufferView;
        std::vector<DirectX12Texture> mTextures;

		Vector2i mSize;
        size_t mSamples;
    };

}
}
#pragma once

#include "directx11rendertarget.h"
#include "Meta/math/matrix4.h"
#include "util/directx11texture.h"

#include "Madgine/render/renderpassflags.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11RenderTexture : DirectX11RenderTarget {

        DirectX11RenderTexture(DirectX11RenderContext *context, const Vector2i &size, const RenderTextureConfig &config);
        ~DirectX11RenderTexture();
                
        bool resizeImpl(const Vector2i &size) override;
        Vector2i size() const override;
        
        virtual void beginIteration(bool flipFlopping, size_t targetIndex, size_t targetCount, size_t targetSubresourceIndex) const override;
        virtual void endIteration() const override;

        virtual void beginFrame() override;

        const DirectX11Texture &getTexture(size_t index) const;
        virtual TextureDescriptor texture(size_t index) const override;     
        virtual size_t textureCount() const override;
        virtual TextureDescriptor depthTexture() const override;

        void blit(RenderTarget *input) const;
        
    private:        
        std::vector<DirectX11Texture> mTextures;

        TextureType mType;
        size_t mSamples;

		Vector2i mSize;
    };

}
}
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
        
        virtual void beginIteration(size_t iteration) const override;
        virtual void endIteration(size_t iteration) const override;

        virtual TextureDescriptor texture(size_t index, size_t iteration = std::numeric_limits<size_t>::max()) const override;     
        virtual size_t textureCount() const override;
        virtual TextureDescriptor depthTexture() const override;
        
    private:        
        std::vector<DirectX11Texture> mTextures;
        std::vector<DirectX11Texture> mMultisampledTextures;

        TextureType mType;
        size_t mSamples;

		Vector2i mSize;
    };

}
}
#pragma once

#include "openglrendertarget.h"
#include "util/opengltexture.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLRenderTexture : OpenGLRenderTarget {

        OpenGLRenderTexture(OpenGLRenderContext *context, const Vector2i &size, const RenderTextureConfig &config);
        ~OpenGLRenderTexture();

        bool resizeImpl(const Vector2i &size) override;
        Vector2i size() const override;

        virtual void beginIteration(bool flipFlopping, size_t targetIndex, size_t targetCount, size_t targetSubresourceIndex) const override;
        virtual void endIteration() const override;

        virtual TextureDescriptor texture(size_t index) const override;
        virtual size_t textureCount() const override;
        virtual TextureDescriptor depthTexture() const override;

        void blit(RenderTarget *input) const;

        virtual Matrix4 getClipSpaceMatrix() const override;

    protected:
        size_t getFramebufferCount(bool *emulateCube = nullptr) const;

    private:
        GLuint mFramebuffers[6] = { 0 };
        GLuint mDepthRenderbuffer = 0;

        OpenGLTexture mDepthTexture;

        size_t mSamples;
        bool mHDR;

        std::vector<OpenGLTexture> mTextures;

        Vector2i mSize;

        TextureType mType;
    };

}
}
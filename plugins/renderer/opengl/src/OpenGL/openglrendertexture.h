#pragma once

#include "Madgine/render/rendertarget.h"
#include "openglshaderprogram.h"
#include "opengltexture.h"

namespace Engine {
	namespace Render {

		struct OpenGLRenderTexture : RenderTarget {

			OpenGLRenderTexture(OpenGLRenderWindow *window, uint32_t index, Camera *camera, const Vector2 &size);
			~OpenGLRenderTexture();


			uint32_t textureId() const override;

			void resize(const Vector2 &size) override;

			virtual void render() override;

		private:
			uint32_t mIndex;

			unsigned int mFramebuffer;
			unsigned int mVertexbuffer;			
			unsigned int mDepthRenderbuffer;

			OpenGLShaderProgram mProgram;

			OpenGLTexture mTexture;

		};

	}
}
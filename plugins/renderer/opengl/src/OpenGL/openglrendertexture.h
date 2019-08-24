#pragma once

#include "Madgine/render/rendertarget.h"
#include "openglshaderprogram.h"
#include "util/opengltexture.h"
#include "util/openglbuffer.h"

namespace Engine {
	namespace Render {

		struct OpenGLRenderTexture : RenderTarget {

			OpenGLRenderTexture(OpenGLRenderWindow *window, uint32_t index, Scene::Camera *camera, const Vector2 &size);
			~OpenGLRenderTexture();


			uint32_t textureId() const override;

			void resize(const Vector2 &size) override;

			virtual void render() override;

		private:
			uint32_t mIndex;

			GLuint mFramebuffer;
			GLuint mDepthRenderbuffer;

			OpenGLShaderProgram mProgram;

			OpenGLTexture mTexture;

		};

	}
}
#pragma once

#include "client/render/rendertarget.h"
#include "openglshaderprogram.h"
#include "opengltexture.h"

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
			GLuint mVertexbuffer;			
			GLuint mDepthRenderbuffer;

			OpenGLShaderProgram mProgram;

			OpenGLTexture mTexture;

			OpenGLRenderWindow *mWindow;

		};

	}
}
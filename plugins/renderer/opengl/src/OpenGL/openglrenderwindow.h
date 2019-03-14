#pragma once

#include "client/render/renderwindow.h"

#include "openglshaderprogram.h"

#include "opengltexture.h"


namespace Engine {
	namespace Render {

		class MADGINE_OPENGL_EXPORT OpenGLRenderWindow : public RenderWindow {
		public:
			OpenGLRenderWindow(GUI::TopLevelWindow *w, ContextHandle context);
			~OpenGLRenderWindow();

			virtual void render() override;

			virtual std::unique_ptr<RenderTarget> createRenderTarget(Camera * camera, const Vector2& size) override;

		private:
			ContextHandle mContext;

			uint32_t mTextureCount = 0;

			OpenGLShaderProgram mProgram;

			OpenGLTexture mDefaultTexture;

		};

	}
}
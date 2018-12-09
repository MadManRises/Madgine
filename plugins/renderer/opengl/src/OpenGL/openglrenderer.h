#pragma once

#include "Madgine/render/renderer.h"

namespace Engine {
	namespace Render {

		class OpenGLRenderer : public Renderer<OpenGLRenderer>
		{

		public:
			OpenGLRenderer(GUI::GUISystem *gui);
			~OpenGLRenderer();

			virtual bool init() override;
			virtual void finalize() override;

			virtual std::unique_ptr<RenderWindow> createWindow(GUI::TopLevelWindow *w) override;			
			
			static void shutdownWindow(Window::Window *window, HGLRC ourOpenGLRenderingContext);

		protected:
			HGLRC setupWindowInternal(Window::Window *window);

		private:
			PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
			PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;		

		};

	}
}

RegisterClass(Engine::Render::OpenGLRenderer);
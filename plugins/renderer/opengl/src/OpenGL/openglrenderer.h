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
			
			static void shutdownWindow(Window::Window *window, ContextHandle ourOpenGLRenderingContext);

		protected:
			ContextHandle setupWindowInternal(Window::Window *window);

		private:
#if _WIN32
			PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
			PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;		
#endif



		};

	}
}

RegisterClass(Engine::Render::OpenGLRenderer);
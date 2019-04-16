#pragma once

#include "render/renderercollector.h"

namespace Engine {
	namespace Render {

		void shutdownWindow(Window::Window *window, ContextHandle context);

		class OpenGLRenderer : public RendererComponent<OpenGLRenderer>
		{

		public:

			OpenGLRenderer(GUI::GUISystem *gui);
			~OpenGLRenderer();

			virtual bool init() override;
			virtual void finalize() override;

			virtual std::unique_ptr<RenderWindow> createWindow(GUI::TopLevelWindow *w) override;			
		};

	}
}

RegisterType(Engine::Render::OpenGLRenderer);
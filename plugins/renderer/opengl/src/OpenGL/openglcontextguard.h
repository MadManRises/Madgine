#pragma once

namespace Engine {
	namespace Render {

		struct MADGINE_OPENGL_EXPORT OpenGLContextGuard {
			OpenGLContextGuard(Window::Window *window, ContextHandle context);
			OpenGLContextGuard(const OpenGLContextGuard&) = delete;
			OpenGLContextGuard(OpenGLContextGuard&& other);
			~OpenGLContextGuard();

		private:
			bool mMovedFrom = false;
			ContextHandle mLastContext;
			Window::Window *mLastWindow;
		};

	}
}

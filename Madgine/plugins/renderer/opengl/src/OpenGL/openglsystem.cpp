#include "opengllib.h"

#include "openglsystem.h"

#include "opengltoplevelwindow.h"

#include "glad.h"


extern "C" DLL_EXPORT Engine::GUI::GUISystem * guisystem(Engine::App::ClientApplication &app) { return new Engine::GUI::OpenGLSystem(app); }

namespace Engine {
	namespace GUI {

		OpenGLSystem::OpenGLSystem(App::ClientApplication & app) :
			GUISystem(app)
		{
		}

		bool OpenGLSystem::init()
		{		

			addTopLevelWindow(std::make_unique<OpenGLTopLevelWindow>(*this));

			return GUISystem::init();;
		}

	}
}



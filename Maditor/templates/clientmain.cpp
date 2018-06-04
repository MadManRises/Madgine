
#include "madginelib.h"
#include "App/OgreApplication.h"
#include "App/Ogreappsettings.h"

int main() {

	Engine::App::OgreAppSettings settings;

	settings.mRootDir = "Media/";
	settings.mWindowName = "%1";

	return Engine::App::Application::run<Engine::App::OgreApplication>(settings);

}
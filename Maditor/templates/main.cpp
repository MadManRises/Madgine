
#include "madginelib.h"
#include "App/Application.h"
#include "App/appsettings.h"

int main() {

	Engine::App::AppSettings settings;

	settings.mRootDir = "";
	settings.mWindowName = "%1";

	return Engine::App::Application::run(settings);

}
#include "Madgine/clientlib.h"

#include "Madgine/app/clientapplication.h"
#include "Madgine/app/clientappsettings.h"
#include "Madgine/core/root.h"

#include <iostream>

int main() {
	int result;
	{
		Engine::Core::Root root;
		root.init();
		Engine::App::ClientAppSettings settings;
		settings.mRunMain = false;
		settings.mAppName = "Madgine Client";
		settings.mWindowSettings.mTitle = "Maditor";
		result = Engine::App::Application::run<Engine::App::ClientApplication>(settings);
	}
	int i;
	std::cin >> i;
	return result;
}
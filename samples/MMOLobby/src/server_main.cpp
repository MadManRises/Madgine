#include <Madgine/baselib.h>
#include <Madgine/server/serverlib.h>
#include <Madgine/clientlib.h>
#include "mmolobbylib.h"

#include "Madgine/base/root.h"
#include "Modules/threading/workgroup.h"
#include "server.h"
#include "Modules/threading/scheduler.h"

#include "Madgine/window/mainwindow.h"

#include "Madgine/base/application.h"

#include "Interfaces/window/windowsettings.h"

int main() {

	Engine::Threading::WorkGroup group("MMOLobby-Server");
	Engine::Base::Root root;

	Engine::Util::StandardLog::setLogLevel(Engine::Util::MessageType::DEBUG_TYPE);

	MMOLobby::Server server;

	Engine::Base::Application app;

	Engine::Window::WindowSettings windowSettings;
	windowSettings.mTitle = "MMOLobby-Server";
	windowSettings.mRestoreGeometry = false;
	Engine::Window::MainWindow window{ windowSettings };

	return Engine::Threading::Scheduler{}.go();
}
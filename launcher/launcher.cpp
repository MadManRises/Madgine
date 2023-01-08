#include "Madgine/applib.h"
#include "Madgine/clientlib.h"
#include "Madgine/uilib.h"

#include "launcher.h"

#include "Interfaces/window/windowsettings.h"
#include "Madgine/app/application.h"
#include "Madgine/root/keyvalueregistry.h"
#include "Madgine/window/mainwindow.h"
#include "Modules/threading/scheduler.h"

#include "Madgine/input/uimanager.h"

#include "launcherconfig.h"

#if EMSCRIPTEN
#    define FIX_LOCAL static
#else
#    define FIX_LOCAL
#endif

#ifndef MADGINE_LAUNCHER_WINDOW_TITLE
#    define MADGINE_LAUNCHER_WINDOW_TITLE "Maditor"
#endif

int launch(std::function<void(Engine::Window::MainWindow &)> callback)
{
    FIX_LOCAL Engine::KeyValueWorkGroupLocal<Engine::App::Application> app { "Application" };

    FIX_LOCAL Engine::Window::WindowSettings windowSettings;
    windowSettings.mTitle = MADGINE_LAUNCHER_WINDOW_TITLE;
    FIX_LOCAL Engine::KeyValueWorkGroupLocal<Engine::Window::MainWindow> window { "MainWindow", windowSettings };

    if (callback)
        callback(window);

    FIX_LOCAL Engine::KeyValueWorkGroupLocal<Engine::Input::UIManager> ui { "UI", app, window };

    FIX_LOCAL Engine::Threading::Scheduler scheduler;
    return scheduler.go();
}

#include "Madgine/baselib.h"
#include "Madgine/clientlib.h"
#include "serialize/filesystem/filesystemlib.h"
#include "Madgine/uilib.h"

#include "launcher.h"

#include "Interfaces/window/windowsettings.h"
#include "Madgine/app/application.h"
#include "Madgine/core/keyvalueregistry.h"
#include "Madgine/window/mainwindow.h"
#include "Meta/serialize/operations.h"
#include "Meta/serialize/streams/formattedserializestream.h"
#include "Modules/threading/scheduler.h"

#include "Meta/serialize/formatter/xmlformatter.h"
#include "serialize/filesystem/filemanager.h"

#include "Meta/serialize/hierarchy/statetransmissionflags.h"

#include "Madgine/resources/resourcemanager.h"

#include "Madgine/input/uimanager.h"

#if EMSCRIPTEN
#    define FIX_LOCAL static
#else
#    define FIX_LOCAL
#endif

int launch(Engine::Window::MainWindow **topLevelPointer)
{
    FIX_LOCAL Engine::KeyValueWorkGroupLocal<Engine::App::Application> app { "Application" };

    FIX_LOCAL Engine::Window::WindowSettings windowSettings;
    windowSettings.mTitle = "Maditor";
    FIX_LOCAL Engine::KeyValueWorkGroupLocal<Engine::Window::MainWindow> window { "MainWindow", windowSettings };

    FIX_LOCAL Engine::KeyValueWorkGroupLocal<Engine::Input::UIManager> ui { "UI", app, window };

    if (topLevelPointer)
        *topLevelPointer = &window;

#if !ENABLE_PLUGINS
    window.taskQueue()
        ->addSetupSteps([&]() {
            Engine::Filesystem::FileManager mgr { "Layout" };
            Engine::Serialize::FormattedSerializeStream file = mgr.openRead(Engine::Resources::ResourceManager::getSingleton().findResourceFile("default.layout"), std::make_unique<Engine::Serialize::XMLFormatter>());

            if (file) {
                Engine::Serialize::read(file, window, nullptr, {}, Engine::Serialize::StateTransmissionFlags_ApplyMap);
                return true;
            } else {
                return false;
            }
        });
#endif

    FIX_LOCAL Engine::Threading::Scheduler scheduler;
    return scheduler.go();
}

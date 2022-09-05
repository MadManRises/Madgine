#include "Madgine/applib.h"
#include "Madgine/clientlib.h"
#include "Madgine/uilib.h"
#include "Madgine/rootlib.h"
#include "Madgine/resourceslib.h"
#include "Madgine/serialize/filesystem/filesystemlib.h"

#include "launcher.h"

#include "Interfaces/window/windowsettings.h"
#include "Madgine/app/application.h"
#include "Madgine/root/keyvalueregistry.h"
#include "Madgine/window/mainwindow.h"
#include "Meta/serialize/operations.h"
#include "Meta/serialize/streams/formattedserializestream.h"
#include "Modules/threading/scheduler.h"

#include "Meta/serialize/formatter/xmlformatter.h"
#include "Madgine/serialize/filesystem/filemanager.h"

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

    if (topLevelPointer)
        *topLevelPointer = &window;

#if !ENABLE_PLUGINS
    window.taskQueue()
        ->addSetupSteps([&]() {
            Engine::Filesystem::FileManager mgr { "Layout" };
            Engine::Serialize::FormattedSerializeStream file = mgr.openRead(Engine::Resources::ResourceManager::getSingleton().findResourceFile("default.layout"), std::make_unique<Engine::Serialize::XMLFormatter>());

            if (file) {
                Engine::Serialize::StreamResult result = Engine::Serialize::read(file, window, nullptr, {}, Engine::Serialize::StateTransmissionFlags_ApplyMap);
                if (result.mState != Engine::Serialize::StreamState::OK) {
                    LOG_ERROR(result);
                    return false;
                }
                return true;
            } else {
                LOG_ERROR("Could not find default.layout!");
                return false;
            }
        });
#endif

    FIX_LOCAL Engine::KeyValueWorkGroupLocal<Engine::Input::UIManager> ui { "UI", app, window };

    FIX_LOCAL Engine::Threading::Scheduler scheduler;
    return scheduler.go();
}

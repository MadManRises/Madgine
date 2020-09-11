#include "Madgine/baselib.h"
#include "Madgine/clientlib.h"

#include "Interfaces/window/windowsettings.h"
#include "Madgine/app/application.h"
#include "Madgine/app/appsettings.h"
#include "Madgine/core/root.h"
#include "Madgine/window/mainwindow.h"
#include "Modules/cli/parameter.h"
#include "Modules/keyvalueutil/keyvalueregistry.h"
#include "Modules/serialize/streams/serializestream.h"
#include "Modules/threading/scheduler.h"
#include "Modules/threading/workgroup.h"

#include "filesystem/filesystemlib.h"
#include "filesystem/filemanager.h"
#include "xml/xmllib.h"
#include "xml/xmlformatter.h"

#include "Modules/resources/resourcemanager.h"

#include "Interfaces/debug/stacktrace.h"
#include "Modules/threading/taskguard.h"

Engine::CLI::Parameter<bool> toolMode { { "--toolMode", "-t" }, false, "If set, no application will be started. Only the root will be initialized and then immediately shutdown again." };

#if EMSCRIPTEN
#    define FIX_LOCAL static
#else
#    define FIX_LOCAL
#endif

int launch(Engine::Window::MainWindow **topLevelPointer = nullptr)
{

    FIX_LOCAL Engine::App::AppSettings settings;

    settings.mAppName = "Madgine Client";
    FIX_LOCAL Engine::KeyValueWorkGroupLocal<Engine::App::Application> app { "Application", settings };

    FIX_LOCAL Engine::Window::WindowSettings windowSettings;
    windowSettings.mTitle = "Maditor";
    FIX_LOCAL Engine::KeyValueWorkGroupLocal<Engine::Window::MainWindow> window { "MainWindow", windowSettings };

    if (topLevelPointer)
        *topLevelPointer = &window;

#if !ENABLE_PLUGINS
    window.frameLoop()
        .addSetupSteps([&]() {
            Engine::Filesystem::FileManager mgr { "Layout" };
            Engine::Serialize::SerializeInStream file = mgr.openRead(Engine::Resources::ResourceManager::getSingleton().findResourceFile("default.layout"), std::make_unique<Engine::XML::XMLFormatter>());

            if (file) {
                window.readState(file);
                return true;
            } else {
                return false;
            }
        });
#endif

    FIX_LOCAL Engine::Threading::Scheduler scheduler;
    return scheduler.go();
}

#if !EMSCRIPTEN
DLL_EXPORT_TAG int main(int argc, char **argv)
{
    Engine::Threading::WorkGroup workGroup { "Launcher" };
    Engine::Core::Root root { "MadgineLauncher", argc, argv };
    if (!toolMode) {
        return launch();
    } else {
        return root.errorCode();
    }
}
#endif

void madgine_terminate_handler()
{
    {
        Engine::Util::LogDummy cout { Engine::Util::ERROR_TYPE };
        cout << "Terminate called! (Madgine-Handler)\n";
        cout << "Stack-Trace:\n";
        for (const Engine::Debug::TraceBack &trace : Engine::Debug::StackTrace<64>::getCurrent(1).calculateReadable())
            cout << trace.mFunction << " (" << trace.mFile << ": " << trace.mLineNr << ")\n";
    }
    abort();
}

static Engine::Threading::TaskGuard global { []() { std::set_terminate(&madgine_terminate_handler); } };

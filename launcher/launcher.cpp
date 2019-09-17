#include "Madgine/baselib.h"

#include "Madgine/app/application.h"
#include "Madgine/app/appsettings.h"
#include "Madgine/core/root.h"
#include "Modules/cli/cli.h"
#include "Modules/threading/workgroup.h"

Engine::CLI::Parameter<bool> toolMode { { "--toolMode", "-t" }, false, "If enabled, no application will be started. Only the root will be initialized and then immediately shutdown again." };

#if EMSCRIPTEN
#    define FIX_LOCAL static
#else
#    define FIX_LOCAL
#endif

int launch(Engine::Threading::WorkGroup &workGroup, Engine::Core::Root &root)
{
    if (!toolMode) {
        FIX_LOCAL Engine::App::AppSettings settings;
        settings.mRunMain = false;
        settings.mAppName = "Madgine Client";
        settings.mWindowSettings.mTitle = "Maditor";
        FIX_LOCAL Engine::App::Application app(settings);
        FIX_LOCAL Engine::Threading::Scheduler scheduler(workGroup, { &app.frameLoop() });
        return scheduler.go();
    } else {
        return root.errorCode();
    }
}

#if !EMSCRIPTEN
DLL_EXPORT_TAG int main(int argc, char **argv)
{
    Engine::Threading::WorkGroup workGroup("Launcher");
    Engine::Core::Root root { argc, argv };
    return launch(workGroup, root);
}
#endif

#include "Madgine/baselib.h"

#include "Madgine/app/application.h"
#include "Madgine/app/appsettings.h"
#include "Madgine/core/root.h"
#include "Modules/cli/cli.h"
#include "Modules/threading/workgroup.h"

#if WINDOWS
#include <conio.h>
#endif

static Engine::CLIOption<bool> toolMode{ { "--toolMode", "-t" }, false, "If enabled, no application will be started. Only the root will be initialized and then immediately shutdown again." };

#if EMSCRIPTEN
#define FIX_LOCAL static
#else
#define FIX_LOCAL
#endif

DLL_EXPORT_TAG int main(int argc, char **argv)
{
    int result;
    {
        FIX_LOCAL Engine::Threading::WorkGroup workGroup("Launcher");
        FIX_LOCAL Engine::Core::Root root{ argc, argv };
        if (!toolMode) {
            FIX_LOCAL Engine::App::AppSettings settings;
            settings.mRunMain = false;
            settings.mAppName = "Madgine Client";
            settings.mWindowSettings.mTitle = "Maditor";
            FIX_LOCAL Engine::App::Application app(settings);
            FIX_LOCAL Engine::Threading::Scheduler scheduler(workGroup, { &app.frameLoop() });
            result = scheduler.go();
        } else {
            result = root.errorCode();
        }
    }
#if WINDOWS
    while (!_kbhit())
        ;
#endif
    return result;
}

#if ANDROID

#include "android/androidlauncher.h"

extern "C" DLL_EXPORT void ANativeActivity_onCreate(ANativeActivity *activity,
    void *savedState, size_t savedStateSize)
{
    new Engine::Android::AndroidLauncher(activity);
}

#endif

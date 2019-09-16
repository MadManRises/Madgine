#include "Madgine/baselib.h"

#include "Interfaces/filesystem/api.h"
#include "Madgine/app/application.h"
#include "Madgine/app/appsettings.h"
#include "Madgine/core/root.h"
#include "Modules/cli/cli.h"
#include "Modules/threading/workgroup.h"

#if WINDOWS
#    include <conio.h>
#endif

Engine::CLI::Parameter<bool> toolMode { { "--toolMode", "-t" }, false, "If enabled, no application will be started. Only the root will be initialized and then immediately shutdown again." };

#if EMSCRIPTEN
#    include <emscripten.h>

#    define FIX_LOCAL static
#else
#    define FIX_LOCAL
#endif

DLL_EXPORT_TAG int main(int argc, char **argv)
{

#if EMSCRIPTEN
    EM_ASM(
        FS.mkdir('/cwd');
        FS.mount(IDBFS, {}, '/cwd');

        FS.syncfs(
            true, function(err) {
                assert(!err);
            }););
    Engine::Filesystem::setCwd("/cwd");

#endif

    int result;
    {
        FIX_LOCAL Engine::Threading::WorkGroup workGroup("Launcher");
#if EMSCRIPTEN
        Engine::Threading::DefaultTaskQueue::getSingleton().addRepeatedTask([]() {
            EM_ASM(
                FS.syncfs(
                    false, function(err) {
                        assert(!err);
                    }););
        },
            std::chrono::seconds { 5 });
#endif
		FIX_LOCAL Engine::Core::Root root { argc, argv };
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
    return result;
}

#if ANDROID

#    include "android/androidlauncher.h"

extern "C" DLL_EXPORT void ANativeActivity_onCreate(ANativeActivity *activity,
    void *savedState, size_t savedStateSize)
{
    new Engine::Android::AndroidLauncher(activity);
}

#endif

#include "Madgine/applib.h"
#include "Madgine/resourceslib.h"

#include "Interfaces/filesystem/fsapi.h"

#include "Modules/threading/workgroup.h"

#include "Madgine/root/root.h"
#include "Modules/threading/scheduler.h"

#include "launcher.h"

#include "main.h"

int desktopMain(int argc, char **argv, std::function<void(Engine::Window::MainWindow &)> callback)
{
    Engine::Filesystem::setup();
    Engine::Threading::WorkGroup workGroup { "Launcher" };

    Engine::Root::Root root { argc, argv };

    if (root.errorCode() != 0)
        return root.errorCode();

    if (root.toolMode()) {
        int result = Engine::Threading::Scheduler {}.go();
        if (result != 0)
            return result;
        return root.errorCode();
    } else {
        return launch(callback);
    }
}

#if !EMSCRIPTEN && !OSX && !IOS && !WINDOWS
DLL_EXPORT_TAG int main(int argc, char **argv)
{
    return desktopMain(argc, argv);
}
#endif
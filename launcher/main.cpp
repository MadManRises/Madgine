#include "Madgine/applib.h"
#include "Madgine/resourceslib.h"

#include "Interfaces/filesystem/api.h"

#include "Modules/threading/workgroup.h"

#include "Madgine/root/root.h"

#include "launcher.h"

#include "main.h"

int desktopMain(int argc, char **argv, std::function<void(Engine::Window::MainWindow&)> callback)
{
    Engine::Filesystem::setup();
    Engine::Threading::WorkGroup workGroup { "Launcher" };


    Engine::Root::Root root { argc, argv };

    if (!root.toolMode()) {
        return launch(callback);
    } else {
        return root.errorCode();
    }
}

#if !EMSCRIPTEN && !OSX && !IOS && !WINDOWS
DLL_EXPORT_TAG int main(int argc, char **argv)
{
    return desktopMain(argc, argv);
}
#endif
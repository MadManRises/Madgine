#include "Madgine/clientlib.h"
#include "Madgine/baselib.h"

#if EMSCRIPTEN

#    include "Interfaces/filesystem/api.h"
#    include "Madgine/core/root.h"
#    include "Modules/threading/workgroup.h"
#include "../launcher.h"
#include "Madgine/app/application.h"
#include <emscripten.h>

void mainImpl()
{
    emscripten_cancel_main_loop();
    static Engine::Threading::WorkGroup workGroup { "Launcher" };
    static Engine::Core::Root root;
    launch();
}

DLL_EXPORT_TAG int main(int argc, char **argv)
{
    void (*callback)() = &mainImpl;
    Engine::Filesystem::setup(&callback);

    emscripten_set_main_loop_arg([](void *) {}, nullptr, 0, false);
    return 0;
}

#endif
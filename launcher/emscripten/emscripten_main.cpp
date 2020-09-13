#include "Madgine/clientlib.h"
#include "Madgine/baselib.h"

#if EMSCRIPTEN

#    include "Interfaces/filesystem/api.h"
#    include "Madgine/core/root.h"
#    include "Modules/threading/defaulttaskqueue.h"
#    include "Modules/threading/workgroup.h"

extern int launch(Engine::Window::MainWindow **topLevelPointer = nullptr);

EMSCRIPTEN_KEEPALIVE DLL_EXPORT_TAG extern "C" int mainImpl()
{
    emscripten_cancel_main_loop();
    static Engine::Threading::WorkGroup workGroup { "Launcher" };
    Engine::Threading::DefaultTaskQueue::getSingleton().addRepeatedTask([]() {
        Engine::Filesystem::sync();
    },
        std::chrono::seconds { 15 });
    static Engine::Core::Root root;
    return launch();
}

DLL_EXPORT_TAG int main(int argc, char **argv)
{
    Engine::Filesystem::setup();

    emscripten_set_main_loop_arg([](void *) {}, nullptr, 0, false);
    return 0;
}

#endif
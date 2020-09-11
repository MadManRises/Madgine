#include "Madgine/clientlib.h"
#include "Madgine/baselib.h"

#if EMSCRIPTEN

#    include "Interfaces/filesystem/api.h"
#    include "Madgine/core/root.h"
#    include "Modules/threading/defaulttaskqueue.h"
#    include "Modules/threading/workgroup.h"
#    include <emscripten.h>

extern int launch(Engine::Window::MainWindow **topLevelPointer = nullptr);

EMSCRIPTEN_KEEPALIVE DLL_EXPORT_TAG extern "C" int mainImpl()
{
    emscripten_cancel_main_loop();
    static Engine::Threading::WorkGroup workGroup { "Launcher" };
    Engine::Threading::DefaultTaskQueue::getSingleton().addRepeatedTask([]() {
        EM_ASM(
            FS.syncfs(
                false, function(err) {
                    assert(!err);
                }););
    },
        std::chrono::seconds { 15 });
    static Engine::Core::Root root;
    return launch();
}

DLL_EXPORT_TAG int main(int argc, char **argv)
{
    EM_ASM(
        FS.mkdir('/cwd');
        FS.mount(IDBFS, {}, '/cwd');

        FS.syncfs(
            true, function(err) {
                assert(!err);
                _mainImpl();
            }););
    Engine::Filesystem::setCwd("/cwd");

    emscripten_set_main_loop_arg([](void *) {}, nullptr, 0, false);
    return 0;
}

#endif
#include "Madgine/clientlib.h"
#include "Madgine/baselib.h"

#if EMSCRIPTEN

#    include "Interfaces/filesystem/api.h"
#    include "Madgine/core/root.h"
#    include "Modules/threading/defaulttaskqueue.h"
#    include "Modules/threading/workgroup.h"
#include "../launcher.h"
#include <emscripten.h>

namespace Engine {
namespace Filesystem {
    extern void sync();
}
}

void mainImpl()
{
    emscripten_cancel_main_loop();
    static Engine::Threading::WorkGroup workGroup { "Launcher" };
    Engine::Threading::DefaultTaskQueue::getSingleton().addRepeatedTask([]() {
        Engine::Filesystem::sync();
    },
        std::chrono::seconds { 15 });
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
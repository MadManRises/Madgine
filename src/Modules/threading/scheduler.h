#pragma once

namespace Engine {
namespace Threading {

    struct MODULES_EXPORT Scheduler {
        Scheduler();

        int go();

        void singleLoop();

    private:
#if !EMSCRIPTEN
        void schedulerLoop(Threading::TaskQueue *queue);
#endif

        void setupThreadInfo(Threading::TaskQueue *queue, std::string tags = "");

    private:
        WorkGroup &mWorkgroup;
    };

}
}
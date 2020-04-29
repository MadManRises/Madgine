#pragma once

namespace Engine {
namespace Threading {

    struct MODULES_EXPORT Scheduler {
        Scheduler(WorkGroup &group);

        int go();

        void singleLoop();

    private:
        void schedulerLoop(Threading::TaskQueue *queue);

    private:
        WorkGroup &mWorkgroup;
    };

}
}
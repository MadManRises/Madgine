#pragma once

namespace Engine {
namespace Threading {

    struct MODULES_EXPORT Scheduler {
        Scheduler(WorkGroup &group, std::vector<Threading::TaskQueue *> additionalQueues = {});

        int go();

        void singleLoop();

    private:
        void schedulerLoop(Threading::TaskQueue *queue);

    private:
        WorkGroup &mWorkgroup;
        std::vector<Threading::TaskQueue *> mAdditionalQueues;
    };

}
}
#pragma once

namespace Engine {
namespace Threading {

    struct MODULES_EXPORT Scheduler {
        Scheduler(WorkGroup &group, std::vector<SignalSlot::TaskQueue *> additionalQueues = {});

        int go();

        void singleLoop();

    private:
        void schedulerLoop(SignalSlot::TaskQueue *queue);

    private:
        WorkGroup &mWorkgroup;
        std::vector<SignalSlot::TaskQueue *> mAdditionalQueues;
    };

}
}
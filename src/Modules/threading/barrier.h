#pragma once

namespace Engine {
namespace Threading {

    struct MODULES_EXPORT Barrier {

        enum Flags {
            NO_FLAGS = 0,
            RUN_ONLY_ON_MAIN_THREAD = 1 << 0
        };

        Barrier(int flags, size_t threadCount);
        Barrier(const Barrier &) = delete;

        Barrier &operator=(const Barrier &) = delete;        

        void enter(TaskQueue *queue, size_t queueIndex, bool isMain);

        void queue(TaskQueue *queue, TaskHandle task);

        bool running();
        bool started();

        void addThread();

    private:
        enum class State {
            INIT,
            RUN,            
            DONE
        };

        std::atomic<State> mState = State::INIT;

        mutable std::mutex mMutex;
        std::condition_variable mCv, mCv_main;
        std::map<TaskQueue *, std::vector<TaskHandle>> mTaskLists;
        std::vector<TaskHandle> mTasks;

        int mFlags;
        size_t mThreadCount;
        std::atomic<size_t> mThreadAccumulator = 0;
    };

}
}
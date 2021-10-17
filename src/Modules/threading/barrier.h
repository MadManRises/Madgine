#pragma once

#include "task.h"

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

        void queueHandle(TaskQueue *queue, TaskHandle task);
        template <typename F, typename... Args>
        auto queue(TaskQueue* queue, F &&f, Args&&... args) {
            auto [fut, handle] = make_task(std::forward<F>(f), std::forward<Args>(args)...).release();
            this->queueHandle(queue, std::move(handle));                
            return std::move(fut);
        }

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
        std::map<TaskQueue *, std::vector<TaskHandle>> mTaskLists;
        std::vector<TaskHandle> mTasks;

        int mFlags;
        size_t mThreadCount;
        std::atomic<size_t> mThreadAccumulator = 0;
    };

}
}
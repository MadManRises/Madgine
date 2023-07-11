#pragma once

namespace Engine {
namespace Threading {

    struct MODULES_EXPORT DataMutex {

        struct MODULES_EXPORT Lock {
            Lock() = default;
            Lock(DataMutex &mutex, AccessMode mode
#if MODULES_ENABLE_TASK_TRACKING
                ,
                Threading::TaskQueue *queue = nullptr
#endif
            );
            Lock(const Lock &) = delete;
            Lock(Lock &&other);
            ~Lock();

            Lock &operator=(Lock &&other);

            bool isHeld() const;

        private:
            DataMutex *mMutex = nullptr;
            AccessMode mMode;
#if MODULES_ENABLE_TASK_TRACKING
            Threading::TaskQueue *mQueue;
#endif
        };

        struct MODULES_EXPORT Awaiter {

            Awaiter(DataMutex *mutex, AccessMode mode);

            bool await_ready() noexcept;
            bool await_suspend(TaskHandle task);
            Lock await_resume();

            bool tryLock(TaskQueue *queue);

        private:
            DataMutex *mMutex;
            AccessMode mMode;
            Lock mLock;
        };

        struct MODULES_EXPORT Moded {
            Moded(DataMutex *mutex, AccessMode mode);

            Lock lock();

            Lock tryLock(TaskQueue *queue);

            Awaiter operator co_await();

        private:
            DataMutex *mMutex;
            AccessMode mMode;
        };

        DataMutex(std::string_view name);

        Moded operator()(AccessMode mode);

        const std::string &name() const;

        Lock tryLock(TaskQueue *queue, AccessMode mode);

    private:
        Lock lock(AccessMode mode);
        void unlockImpl(AccessMode mode
#if MODULES_ENABLE_TASK_TRACKING
            ,
            TaskQueue *queue
#endif
        );

    private:
        std::shared_mutex mMutex;
        std::string mName;
    };

}
}
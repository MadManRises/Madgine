#pragma once

#include "Interfaces/debug/stacktrace.h"

namespace Engine {
namespace Debug {

    namespace Threading {

        MODULES_EXPORT void onAssign(const std::coroutine_handle<> &handle, Engine::Threading::TaskQueue *queue, StackTrace<1> stacktrace);
        MODULES_EXPORT void onDestroy(Engine::Threading::TaskSuspendablePromiseTypeBase &promise);

        MODULES_EXPORT void onResume(const Engine::Threading::TaskHandle &handle);
        MODULES_EXPORT void onSuspend(Engine::Threading::TaskQueue *queue);

        MODULES_EXPORT void onEnter(const std::coroutine_handle<> &handle, Engine::Threading::TaskQueue *queue);
        MODULES_EXPORT void onReturn(const std::coroutine_handle<> &handle, Engine::Threading::TaskQueue *queue);

        MODULES_EXPORT void onLock(Engine::Threading::DataMutex *mutex, Engine::AccessMode mode, Engine::Threading::TaskQueue *queue);
        MODULES_EXPORT void onUnlock(Engine::Threading::DataMutex *mutex, Engine::AccessMode mode, Engine::Threading::TaskQueue *queue);

        struct MODULES_EXPORT TaskTracker {

            void onAssign(void *ident, StackTrace<1> stacktrace);
            void onEnter(void *ident);
            void onReturn(void *ident);
            void onResume(void *ident);
            void onSuspend();
            void onDestroy(void *ident);
            void onLock(Engine::Threading::DataMutex *mutex, Engine::AccessMode mode);
            void onUnlock(Engine::Threading::DataMutex *mutex, Engine::AccessMode mode);

            struct Event {
                enum Type {
                    ASSIGN,
                    ENTER,
                    RETURN,
                    RESUME,
                    SUSPEND,
                    DESTROY,
                    LOCK_MUTEX,
                    UNLOCK_MUTEX
                } mType;
                std::chrono::high_resolution_clock::time_point mTimePoint = std::chrono::high_resolution_clock::now();
                union {
                    std::thread::id mThread;
                    StackTrace<1> mStackTrace;
                    AccessMode mLockMode;
                };
                union {
                    void *mIdentifier = nullptr;
                    Engine::Threading::DataMutex *mMutex;
                };

                Event(Type type, void *ident, StackTrace<1> stacktrace)
                    : mType(type)
                    , mIdentifier(ident)
                    , mStackTrace(stacktrace)
                {
                }

                Event(Type type, void *ident, std::thread::id thread)
                    : mType(type)
                    , mIdentifier(ident)
                    , mThread(thread)
                {
                }

                Event(Type type, std::thread::id thread)
                    : mType(type)
                    , mThread(thread)
                {
                }

                Event(Type type, void *ident)
                    : mType(type)
                    , mIdentifier(ident)
                {
                }

                Event(Type type, Engine::Threading::DataMutex *mutex, AccessMode mode)
                    : mType(type)
                    , mMutex(mutex)
                    , mLockMode(mode)
                {
                }
            };

            const std::deque<Event> &events() const;
            const std::map<void *, StackTrace<1>> tasksInFlight() const;

            std::mutex mMutex;

            std::thread::id mThread;

        private:
            std::deque<Event> mEvents;
            std::map<void *, StackTrace<1>> mTasksInFlight;
        };

    }

}
}
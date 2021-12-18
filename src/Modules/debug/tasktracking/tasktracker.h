#pragma once

#include "Interfaces/debug/stacktrace.h"

namespace Engine {
namespace Debug {

    namespace Threading {

        MODULES_EXPORT void onAssign(std::coroutine_handle<Engine::Threading::TaskPromiseTypeBase> handle, StackTrace<2> stacktrace);
        MODULES_EXPORT void onDestroy(Engine::Threading::TaskPromiseTypeBase &promise);        

        MODULES_EXPORT void onResume(const Engine::Threading::TaskHandle &handle);
        MODULES_EXPORT void onSuspend(Engine::Threading::TaskQueue *queue);

        MODULES_EXPORT void onEnter(const Engine::Threading::TaskHandle &handle);
        MODULES_EXPORT void onReturn(const Engine::Threading::TaskHandle &handle);

        struct MODULES_EXPORT TaskTracker {

            void onAssign(void *ident, StackTrace<2> stacktrace);
            void onEnter(void *ident);
            void onReturn(void *ident);
            void onResume(void *ident);
            void onSuspend();
            void onDestroy(void *ident);

            struct Event {
                enum Type {
                    ASSIGN,
                    ENTER,
                    RETURN,
                    RESUME,
                    SUSPEND,
                    DESTROY
                } mType;
                std::chrono::high_resolution_clock::time_point mTimePoint = std::chrono::high_resolution_clock::now();
                union {
                    std::thread::id mThread;
                    StackTrace<2> mStackTrace;
                };
                void *mIdentifier = nullptr;

                Event(Type type, void *ident, StackTrace<2> stacktrace)
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
            };

            const std::deque<Event> &events() const;

            std::mutex mMutex;

        private:
            std::deque<Event> mEvents;            
        };

    }

}
}
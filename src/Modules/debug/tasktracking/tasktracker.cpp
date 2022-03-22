#include "../../moduleslib.h"

#include "tasktracker.h"

#include "../../threading/taskpromise.h"
#include "../../threading/taskqueue.h"

namespace Engine {
namespace Debug {

    namespace Threading {

        void TaskTracker::onAssign(void *ident, StackTrace<1> stacktrace)
        {
            std::lock_guard guard { mMutex };
            mEvents.emplace_back(Event::ASSIGN, ident, stacktrace);
        }

        void TaskTracker::onEnter(void *ident)
        {
            std::lock_guard guard { mMutex };
            mEvents.emplace_back(Event::ENTER, ident, std::this_thread::get_id());
        }

        void TaskTracker::onReturn(void *ident)
        {
            std::lock_guard guard { mMutex };
            mEvents.emplace_back(Event::RETURN, ident, std::this_thread::get_id());
        }

        void TaskTracker::onResume(void *ident)
        {
            std::lock_guard guard { mMutex };
            mEvents.emplace_back(Event::RESUME, ident, std::this_thread::get_id());
        }

        void TaskTracker::onSuspend()
        {
            std::lock_guard guard { mMutex };
            mEvents.emplace_back(Event::SUSPEND, std::this_thread::get_id());
        }

        void TaskTracker::onDestroy(void *ident)
        {
            std::lock_guard guard { mMutex };
            mEvents.emplace_back(Event::DESTROY, ident);
        }

        const std::deque<TaskTracker::Event> &TaskTracker::events() const
        {
            return mEvents;
        }

        void onAssign(const std::coroutine_handle<> &handle, Engine::Threading::TaskQueue *queue, StackTrace<1> stacktrace)
        {
            queue->mTracker.onAssign(handle.address(), stacktrace);
        }

        void onEnter(const std::coroutine_handle<> &handle, Engine::Threading::TaskQueue *queue)
        {
            queue->mTracker.onEnter(handle.address());
        }

        void onReturn(const std::coroutine_handle<> &handle, Engine::Threading::TaskQueue *queue)
        {
            queue->mTracker.onReturn(handle.address());
        }

        void onResume(const Engine::Threading::TaskHandle &handle)
        {
            handle.queue()->mTracker.onResume(handle.address());
        }

        void onSuspend(Engine::Threading::TaskQueue *queue)
        {
            queue->mTracker.onSuspend();
        }

        void onDestroy(Engine::Threading::TaskPromiseTypeBase &promise)
        {
            promise.queue()->mTracker.onDestroy(std::coroutine_handle<Engine::Threading::TaskPromiseTypeBase>::from_promise(promise).address());
        }

    }

}
}
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
            auto pib = mTasksInFlight.try_emplace(ident, stacktrace);
            assert(pib.second);
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
            //mEvents.emplace_back(Event::DESTROY, ident);
            auto count = mTasksInFlight.erase(ident);
            assert(count == 1);
        }

        void TaskTracker::onLock(Engine::Threading::DataMutex *mutex, Engine::AccessMode mode)
        {
            std::lock_guard guard { mMutex };
            mEvents.emplace_back(Event::LOCK_MUTEX, mutex, mode);
        }

        void TaskTracker::onUnlock(Engine::Threading::DataMutex *mutex, Engine::AccessMode mode)
        {
            std::lock_guard guard { mMutex };
            mEvents.emplace_back(Event::UNLOCK_MUTEX, mutex, mode);
        }

        const std::deque<TaskTracker::Event> &TaskTracker::events() const
        {
            return mEvents;
        }

        const std::map<void *, StackTrace<1>> TaskTracker::tasksInFlight() const
        {
            return mTasksInFlight;
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

        void onLock(Engine::Threading::DataMutex *mutex, Engine::AccessMode mode, Engine::Threading::TaskQueue *queue)
        {
            queue->mTracker.onLock(mutex, mode);
        }

        void onUnlock(Engine::Threading::DataMutex *mutex, Engine::AccessMode mode, Engine::Threading::TaskQueue *queue)
        {
            queue->mTracker.onUnlock(mutex, mode);
        }

        void onResume(const Engine::Threading::TaskHandle &handle)
        {
            handle.queue()->mTracker.onResume(handle.address());
        }

        void onSuspend(Engine::Threading::TaskQueue *queue)
        {
            queue->mTracker.onSuspend();
        }

        void onDestroy(Engine::Threading::TaskSuspendablePromiseTypeBase &promise)
        {
            promise.queue()->mTracker.onDestroy(std::coroutine_handle<Engine::Threading::TaskSuspendablePromiseTypeBase>::from_promise(promise).address());
        }

    }

}
}
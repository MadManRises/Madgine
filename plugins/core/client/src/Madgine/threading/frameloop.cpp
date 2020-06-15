#include "../clientlib.h"

#include "framelistener.h"
#include "frameloop.h"

#include "Modules/threading/taskqueue.h"

#include "Modules/debug/profiler/profile.h"

#include "Modules/generic/container/safeiterator.h"

namespace Engine {
namespace Threading {
    FrameLoop::FrameLoop()
        : TaskQueue("FrameLoop")
        , mSetupState(mSetupSteps.begin())
        , mLastFrame(std::chrono::high_resolution_clock::now())
    {
        addRepeatedTask([this]() {
            auto now = std::chrono::high_resolution_clock::now();
            if (!singleFrame(std::chrono::duration_cast<std::chrono::microseconds>(now - mLastFrame)))
                stop();
            mLastFrame = now;
        },
            std::chrono::microseconds(1000000 / 60), this);
    }

    FrameLoop::~FrameLoop()
    {
        removeRepeatedTasks(this);
    }

    bool FrameLoop::singleFrame(std::chrono::microseconds timeSinceLastFrame)
    {
        return sendFrameStarted(timeSinceLastFrame) && sendFrameRenderingQueued(timeSinceLastFrame) && sendFrameEnded(timeSinceLastFrame);
    }

    void FrameLoop::addFrameListener(FrameListener *listener)
    {
        mListeners.push_back(listener);
    }

    void FrameLoop::removeFrameListener(FrameListener *listener)
    {
        mListeners.erase(std::remove(mListeners.begin(), mListeners.end(), listener), mListeners.end());
    }

    void FrameLoop::shutdown()
    {
        stop();
    }

    bool FrameLoop::isShutdown() const
    {
        return !running();
    }

    bool FrameLoop::sendFrameStarted(std::chrono::microseconds timeSinceLastFrame)
    {
        PROFILE();

        for (FrameListener *listener : safeIterate(mListeners)) {
            if (!listener->frameStarted(timeSinceLastFrame))
                return false;
        }
        return true;
    }

    bool FrameLoop::sendFrameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Threading::ContextMask context)
    {
        PROFILE();
        for (FrameListener *listener : safeIterate(mListeners)) {
            if (!listener->frameRenderingQueued(timeSinceLastFrame, context))
                return false;
        }

        mTimeBank += timeSinceLastFrame;

        while (mTimeBank >= FIXED_TIMESTEP) {
            if (!sendFrameFixedUpdate(FIXED_TIMESTEP))
                return false;
            mTimeBank -= FIXED_TIMESTEP;
        }

        return true;
    }

    bool FrameLoop::sendFrameFixedUpdate(std::chrono::microseconds timeSinceLastFrame, Threading::ContextMask context)
    {
        PROFILE();
        for (FrameListener *listener : safeIterate(mListeners))
            if (!listener->frameFixedUpdate(timeSinceLastFrame, context))
                return false;
        return true;
    }

    std::chrono::nanoseconds FrameLoop::fixedRemainder() const
    {
        return FIXED_TIMESTEP - mTimeBank;
    }

    bool FrameLoop::sendFrameEnded(std::chrono::microseconds timeSinceLastFrame)
    {
        PROFILE();
        for (FrameListener *listener : safeIterate(mListeners)) {
            if (!listener->frameEnded(timeSinceLastFrame))
                return false;
        }
        return true;
    }

    std::optional<Threading::TaskTracker> FrameLoop::fetch(std::chrono::steady_clock::time_point &nextTask, int &idleCount)
    {
        if (running()) {
            while (mSetupState != mSetupSteps.end()) {
                std::optional<Threading::TaskHandle> init = std::move(mSetupState->first);
                ++mSetupState;
                if (init) {
                    return wrapTask(std::move(*init));
                }
            }
        }
        if (std::optional<Threading::TaskTracker> task = TaskQueue::fetch(nextTask, idleCount)) {
            return task;
        }
        if (!running() && nextTask == std::chrono::steady_clock::time_point::max()) {

            while (mSetupState != mSetupSteps.begin()) {
                --mSetupState;
                Threading::TaskHandle finalize = std::move(mSetupState->second);
                if (finalize) {
                    return wrapTask(std::move(finalize));
                }
            }
        }
        return {};
    }

    std::optional<Threading::TaskTracker> FrameLoop::fetch_on_idle()
    {
        /*return wrapTask([this]() {
            auto now = std::chrono::high_resolution_clock::now();
            if (!singleFrame(std::chrono::duration_cast<std::chrono::microseconds>(now - mLastFrame)))
                stop();
            mLastFrame = now;
        });*/
        return {};
    }

    bool FrameLoop::idle() const
    {
        return TaskQueue::idle() && mSetupState == mSetupSteps.begin();
    }

    void FrameLoop::addSetupSteps(Threading::TaskHandle &&init, Threading::TaskHandle &&finalize)
    {
        bool isItEnd = mSetupState == mSetupSteps.end();
        if (init && finalize) {
            std::promise<bool> p;
            std::future<bool> f = p.get_future();
            mSetupSteps.emplace_back(
                [init { std::move(init) }, p { std::move(p) }]() mutable {
                    Threading::TaskState state;
                    try {
                        state = init();
                        assert(state == Threading::SUCCESS || state == Threading::FAILURE);
                        p.set_value(state == Threading::SUCCESS);
                    } catch (std::exception &) {
                        p.set_value(false);
                        throw;
                    }
                    return state;
                },
                [finalize { std::move(finalize) }, f { std::move(f) }]() mutable {
                    if (f.get())
                        return finalize();
                    return Threading::SUCCESS;
                });
        } else {
            mSetupSteps.emplace_back(std::move(init), std::move(finalize));
        }
        if (isItEnd) {
            mSetupState = std::prev(mSetupState);
        }
    }

}
}
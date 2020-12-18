#include "../clientlib.h"

#include "framelistener.h"
#include "frameloop.h"

#include "Modules/debug/profiler/profile.h"

#include "Modules/generic/container/safeiterator.h"

namespace Engine {
namespace Threading {
    FrameLoop::FrameLoop()
        : TaskQueue("FrameLoop", true)        
        , mLastFrame(std::chrono::high_resolution_clock::now())
    {
        addRepeatedTask([this]() {
            auto now = std::chrono::high_resolution_clock::now();
            if (!singleFrame(std::chrono::duration_cast<std::chrono::microseconds>(now - mLastFrame)))
                stop();
            mLastFrame = now;
        },
            Threading::TaskMask::ALL, std::chrono::microseconds(1000000 / 60), this);
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
    

}
}
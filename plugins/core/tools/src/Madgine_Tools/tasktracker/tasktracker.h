#pragma once

#include "../toolbase.h"
#include "../toolscollector.h"

#include "Meta/math/rect2.h"

namespace Engine {
namespace Tools {

    struct TaskTracker : Tool<TaskTracker> {
        SERIALIZABLEUNIT(TaskTracker)

        TaskTracker(ImRoot &root);
        ~TaskTracker();

        virtual void render() override;

        std::string_view key() const override;

    protected:
        Rect2 beginPlot();
        void endPlot();

        float getEventCoordinate(std::chrono::high_resolution_clock::time_point t, float pixelWidth);

    private:
        std::chrono::high_resolution_clock::time_point mStart = std::chrono::high_resolution_clock::now() - std::chrono::milliseconds { 10 }, mEnd = mStart;
        long long mScroll = 0;
        bool mLocked = false;
        float mZoom = 10.0f;

        std::chrono::high_resolution_clock::time_point mHoveredAssignTimepoint;
        Threading::TaskQueue *mHoveredTaskQueue = nullptr;
        void *mHoveredId = nullptr;
    };

}
}

REGISTER_TYPE(Engine::Tools::TaskTracker)
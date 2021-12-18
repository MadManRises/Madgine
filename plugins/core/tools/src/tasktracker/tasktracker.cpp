#include "../toolslib.h"

#include "tasktracker.h"

#include "imgui/imgui.h"
#include "imgui/imguiaddons.h"

#include "Meta/serialize/serializetable_impl.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Modules/threading/taskqueue.h"
#include "Modules/threading/workgroup.h"

#include "imgui/imgui_internal.h"

UNIQUECOMPONENT(Engine::Tools::TaskTracker);

METATABLE_BEGIN_BASE(Engine::Tools::TaskTracker, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::TaskTracker)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::TaskTracker, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::TaskTracker)

namespace Engine {
namespace Tools {

    static constexpr ImU32 colors[4] = {
        IM_COL32(200, 50, 50, 255),
        IM_COL32(150, 150, 50, 255),
        IM_COL32(50, 50, 200, 255),
        IM_COL32(50, 150, 150, 255)
    };

    TaskTracker::TaskTracker(ImRoot &root)
        : Tool<TaskTracker>(root)
    {
    }

    TaskTracker::~TaskTracker()
    {
    }

    void TaskTracker::render()
    {
        if (ImGui::Begin("TaskTracker", &mVisible)) {

            ImGui::Checkbox("Locked", &mLocked);
            if (!mLocked)
                mEnd = std::chrono::high_resolution_clock::now();

            float fullPlotSize = std::chrono::duration_cast<std::chrono::duration<float>>(mEnd - mStart).count();
            ImDrawList *draw_list = ImGui::GetWindowDrawList();

            ImGuiIO &io = ImGui::GetIO();

            bool isHovered = false;

            Threading::TaskQueue *hoveredTaskQueue = nullptr;
            void *hoveredId = nullptr;
            bool renderHovered = true;
            Debug::StackTrace<2> hoveredTraceback;

            std::chrono::high_resolution_clock::time_point timeAreaBegin = mStart + std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(std::chrono::duration<float> { mScroll / 1000.0f });
            std::chrono::high_resolution_clock::time_point timeAreaEnd = timeAreaBegin + std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(std::chrono::duration<float> { 1.0f / mZoom });

            ImGui::Columns(2);

            for (Threading::TaskQueue *queue : Threading::WorkGroup::self().taskQueues()) {
                ImGui::Text(queue->name());

                ImGui::NextColumn();

                beginPlot();

                isHovered |= ImGui::IsItemHovered(); // Hovered

                ImGui::NextColumn();

                ImGui::Text("\tThread [%llu]", std::this_thread::get_id());

                ImGui::NextColumn();

                Rect2 plotRect = beginPlot();

                isHovered |= ImGui::IsItemHovered(); // Hovered

                std::lock_guard guard { queue->mTracker.mMutex };
                auto begin = queue->mTracker.events().begin();
                auto end = queue->mTracker.events().end();
                auto cmp1 = [](const Debug::Threading::TaskTracker::Event &event, const std::chrono::high_resolution_clock::time_point &t) { return event.mTimePoint < t; };
                auto it = std::lower_bound(begin, end, timeAreaBegin, cmp1);
                auto cmp2 = [](const std::chrono::high_resolution_clock::time_point &t, const Debug::Threading::TaskTracker::Event &event) { return t < event.mTimePoint; };
                int i = std::distance(begin, it) % 4;
                end = std::upper_bound(it, end, timeAreaEnd, cmp2);

                while (it != end) {
                    const Debug::Threading::TaskTracker::Event &ev = *it;
                    switch (ev.mType) {
                    case Debug::Threading::TaskTracker::Event::RESUME: {
                    
                        float x = getEventCoordinate(ev.mTimePoint, plotRect.mSize.x);
                        float x_end = plotRect.mSize.x;
                        auto it2 = std::next(it);
                        bool loop = true;
                        while (it2 != end && loop) {
                            const Debug::Threading::TaskTracker::Event &ev2 = *it2;
                            switch (ev2.mType) {
                            case Debug::Threading::TaskTracker::Event::SUSPEND:
                                x_end = getEventCoordinate(ev2.mTimePoint, plotRect.mSize.x);
                                loop = false;
                                break;
                            case Debug::Threading::TaskTracker::Event::RESUME:
                                throw 0;
                                break;
                            }
                            ++it2;
                        }
                        x_end = std::max(x_end, x + 1.0f);
                        ImU32 color = colors[i];
                        if (mHoveredTaskQueue == queue && mHoveredId == ev.mIdentifier)
                            color += IM_COL32(50, 50, 50, 0);
                        draw_list->AddRectFilled({ plotRect.mTopLeft.x + x, plotRect.mTopLeft.y }, { plotRect.mTopLeft.x + x_end, plotRect.bottom() }, color);
                        ImGui::SetCursorScreenPos({ plotRect.mTopLeft.x + x, plotRect.mTopLeft.y });
                        ImGui::InvisibleButton("id", { x_end - x, plotRect.mSize.y }, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

                        if (ImGui::IsItemHovered()) {
                            hoveredTaskQueue = queue;
                            hoveredId = ev.mIdentifier;
                        }

                        i = (i + 1) % 4;
                        break;
                        }
                    case Debug::Threading::TaskTracker::Event::ASSIGN:
                        if (mHoveredTaskQueue == queue && mHoveredId == ev.mIdentifier) {
                            hoveredTraceback = ev.mStackTrace;
                        }
                        break;
                    }
                    ++it;
                }

                ImGui::NextColumn();
            }

            //ImGui::Text("Scroll");

            ImGui::NextColumn();

            ImVec2 canvas_p0 = ImGui::GetCursorScreenPos(); // ImDrawList API uses screen coordinates!
            ImVec2 scrollbar_sz = { canvas_p0.x + ImGui::GetContentRegionAvail().x, canvas_p0.y + 20.0f }; // Resize canvas to what's available

            ImGui::ScrollbarEx({ canvas_p0, scrollbar_sz }, 1, ImGuiAxis_X, &mScroll, 1000.0f / mZoom, 1000.0f * fullPlotSize, ImDrawFlags_RoundCornersAll);

            ImGui::EndColumns();

            ImGui::Text("%f < %f < %f", 0.0f, mScroll, fullPlotSize);

            if (isHovered) {
                mZoom *= powf(1.1f, io.MouseWheel);
            }

            if (mHoveredTaskQueue != hoveredTaskQueue || mHoveredId != hoveredId) {
                mHoveredTaskQueue = hoveredTaskQueue;
                mHoveredId = hoveredId;
                mHoveredAssignTimepoint = mStart;
                renderHovered = false;
            }

            if (renderHovered && mHoveredId) {
                ImGui::BeginTooltip();
                for (Debug::TraceBack& tb : hoveredTraceback.calculateReadable()) {
                    ImGui::Text(tb.mFunction);
                }
                ImGui::EndTooltip();
            }
        }
        ImGui::End();
    }

    std::string_view TaskTracker::key() const
    {
        return "TaskTracker";
    }

    Rect2 TaskTracker::beginPlot()
    {
        // Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
        Vector2 canvas_p0 = ImGui::GetCursorScreenPos(); // ImDrawList API uses screen coordinates!
        Vector2 canvas_sz = { ImGui::GetContentRegionAvail().x, 50.0f }; // Resize canvas to what's available
        if (canvas_sz.x < 50.0f)
            canvas_sz.x = 50.0f;
        Vector2 canvas_p1 = canvas_p0 + canvas_sz;

        // Draw border and background color
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
        draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

        // This will catch our interactions
        ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

        return { canvas_p0 + Vector2 { 2, 2 }, canvas_sz - Vector2 { 4, 4 } };
    }

    void TaskTracker::endPlot()
    {
    }

    float TaskTracker::getEventCoordinate(std::chrono::high_resolution_clock::time_point t, float pixelWidth)
    {
        float timePoint = std::chrono::duration_cast<std::chrono::duration<float>>(t - mStart).count() - mScroll / 1000.0f;
        return (timePoint * mZoom) * pixelWidth;
    }
}
}

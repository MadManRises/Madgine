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

            long long fullPlotSize = std::chrono::duration_cast<std::chrono::nanoseconds>(mEnd - mStart).count();
            ImDrawList *draw_list = ImGui::GetWindowDrawList();

            ImGuiIO &io = ImGui::GetIO();

            bool isHovered = false;
            float mouseRatio;

            Threading::TaskQueue *hoveredTaskQueue = nullptr;
            void *hoveredId = nullptr;
            bool renderHovered = true;
            Debug::StackTrace<1> hoveredTraceback;

            std::chrono::high_resolution_clock::time_point timeAreaBegin = mStart + std::chrono::nanoseconds { mScroll };
            std::chrono::high_resolution_clock::time_point timeAreaEnd = timeAreaBegin + std::chrono::nanoseconds{ static_cast<long long>(1000000000 / mZoom) };

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
                mouseRatio = (ImGui::GetMousePos().x - plotRect.mTopLeft.x) / plotRect.mSize.x;
                ImVec2 keep = ImGui::GetCursorScreenPos();

                isHovered |= ImGui::IsItemHovered(); // Hovered
                bool foundTraceback = false;

                std::lock_guard guard { queue->mTracker.mMutex };
                auto begin = queue->mTracker.events().begin();
                auto end = queue->mTracker.events().end();
                auto cmp1 = [](const Debug::Threading::TaskTracker::Event &event, const std::chrono::high_resolution_clock::time_point &t) { return event.mTimePoint < t; };
                auto start = std::lower_bound(begin, end, timeAreaBegin, cmp1);
                auto cmp2 = [](const std::chrono::high_resolution_clock::time_point &t, const Debug::Threading::TaskTracker::Event &event) { return t < event.mTimePoint; };
                end = std::upper_bound(start, end, timeAreaEnd, cmp2);

                bool found = false;

                for (auto it = start; it != end; ++it) {
                    if (it->mType != Debug::Threading::TaskTracker::Event::ASSIGN) {
                        found = it->mType == Debug::Threading::TaskTracker::Event::RESUME;
                        break;
                    }
                }

                auto it = start;
                if (!found) {
                    while (it != begin && it->mType != Debug::Threading::TaskTracker::Event::RESUME)
                        --it;
                }

                struct Plot {
                    float x;
                    float x_end;
                    void *id;
                    size_t depth;
                };
                int i = 0;
                auto plot = [&](Plot &p) {
                    p.x_end = std::max(p.x_end, p.x + 1.0f);

                    ImU32 color = colors[(i++ + p.depth) % 4];
                    if (mHoveredTaskQueue == queue && mHoveredId == p.id)
                        color += IM_COL32(50, 50, 50, 0);
                    draw_list->AddRectFilled({ plotRect.mTopLeft.x + p.x, plotRect.mTopLeft.y }, { plotRect.mTopLeft.x + p.x_end, plotRect.bottom() - 5.0f * p.depth }, color);
                    ImGui::SetCursorScreenPos({ plotRect.mTopLeft.x + p.x, plotRect.mTopLeft.y });
                    ImGui::InvisibleButton("id", { p.x_end - p.x, plotRect.mSize.y - 5.0f * p.depth }, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

                    if (ImGui::IsItemHovered()) {
                        hoveredTaskQueue = queue;
                        hoveredId = p.id;
                    }
                };

                Plot p {
                    0.0f,
                    plotRect.mSize.x,
                    nullptr,
                    0
                };
                bool suspended = true;
                std::stack<Plot> plots;
                std::stack<std::pair<float, void *>> xs;

                for (; it != end; ++it) {
                    const Debug::Threading::TaskTracker::Event &ev = *it;
                    switch (ev.mType) {
                    case Debug::Threading::TaskTracker::Event::ASSIGN:
                        if (mHoveredTaskQueue == queue && mHoveredId == ev.mIdentifier) {
                            foundTraceback = true;
                            hoveredTraceback = ev.mStackTrace;
                        }
                        break;
                    case Debug::Threading::TaskTracker::Event::RESUME:
                        p.x = getEventCoordinate(ev.mTimePoint, plotRect.mSize.x);
                        p.id = ev.mIdentifier;
                        assert(suspended);
                        suspended = false;
                        break;
                    case Debug::Threading::TaskTracker::Event::SUSPEND:
                        assert(xs.empty());
                        p.x_end = getEventCoordinate(ev.mTimePoint, plotRect.mSize.x);
                        plots.push(p);
                        p.x = 0.0f;
                        p.x_end = plotRect.mSize.x;
                        p.id = nullptr;
                        assert(!suspended);
                        suspended = true;
                        break;
                    case Debug::Threading::TaskTracker::Event::ENTER:
                        suspended = false;
                        xs.push({ getEventCoordinate(ev.mTimePoint, plotRect.mSize.x), ev.mIdentifier });
                        break;
                    case Debug::Threading::TaskTracker::Event::RETURN:
                        float x = 0.0f;
                        if (!xs.empty()) {
                            assert(xs.top().second == ev.mIdentifier);
                            x = xs.top().first;
                            xs.pop();
                        }
                        plots.push({ x, getEventCoordinate(ev.mTimePoint, plotRect.mSize.x), ev.mIdentifier, xs.size() + 1 });
                        break;
                    }
                }
                assert(!suspended || xs.empty());

                while (!xs.empty()) {
                    plots.push({ xs.top().first, plotRect.mSize.x, xs.top().second, xs.size() });
                    xs.pop();
                }

                if (!suspended)
                    plot(p);
                while (!plots.empty()) {
                    plot(plots.top());
                    plots.pop();
                }

                ImGui::SetCursorScreenPos(keep);

                ImGui::NextColumn();

                if (!foundTraceback && mHoveredTaskQueue == queue) {
                    it = start;
                    while (it != begin) {
                        --it;
                        if (it->mType == Debug::Threading::TaskTracker::Event::ASSIGN && it->mIdentifier == mHoveredId) {
                            hoveredTraceback = it->mStackTrace;
                            break;
                        }
                    }
                }
            }

            //ImGui::Text("Scroll");
            ImGui::Text("%f", mouseRatio);

            ImGui::NextColumn();

            ImVec2 canvas_p0 = ImGui::GetCursorScreenPos(); // ImDrawList API uses screen coordinates!
            ImVec2 scrollbar_sz = { canvas_p0.x + ImGui::GetContentRegionAvail().x, canvas_p0.y + 20.0f }; // Resize canvas to what's available

            ImGui::ScrollbarEx({ canvas_p0, scrollbar_sz }, 1, ImGuiAxis_X, &mScroll, 1000000000 / mZoom, fullPlotSize, ImDrawFlags_RoundCornersAll);

            ImGui::EndColumns();

            ImGui::Text("%lld < %lld < %lld", 0, mScroll, fullPlotSize);
            ImGui::DragScalar("debug", ImGuiDataType_S64, &mScroll);

            if (isHovered) {
                float factor = powf(1.1f, io.MouseWheel);
                mZoom *= factor;
                long long mouseOffset = std::chrono::duration_cast<std::chrono::nanoseconds>(timeAreaEnd - timeAreaBegin).count() * mouseRatio;
                mScroll += mouseOffset * (1.0f - 1.0f / factor);
            }

            if (mHoveredTaskQueue != hoveredTaskQueue || mHoveredId != hoveredId) {
                mHoveredTaskQueue = hoveredTaskQueue;
                mHoveredId = hoveredId;
                mHoveredAssignTimepoint = mStart;
                renderHovered = false;
            }

            if (renderHovered && mHoveredId) {
                ImGui::BeginTooltip();
                for (Debug::TraceBack &tb : hoveredTraceback.calculateReadable()) {
                    ImGui::Text("%s", tb.mFunction);
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
        long long timePoint = std::chrono::duration_cast<std::chrono::nanoseconds>(t - mStart).count() - mScroll;
        if (timePoint < 0)
            timePoint = 0;
        return ((timePoint / 1000000000.0f) * mZoom) * pixelWidth;
    }
}
}

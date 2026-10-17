#include "../debugtoolslib.h"

#include "continuationlist.h"

#include "Madgine/debug/continuation.h"

#include "Madgine_Tools/imguiicons.h"
#include "debuggerview.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imguiaddons.h"

namespace Engine {
namespace Tools {

    ContinuationList::ContinuationList(const Debug::ContextInfo &context, ControlButton button)
        : mContext(context)
        , mButton(button)
    {
    }

    ContinuationList::~ContinuationList()
    {
        assert(!mBreakpoint);
        assert(!mIsMarker);

        for (auto &[continuation, mode, callback] : mContinuations) {
            if (callback)
                callback();
            continuation(mode);
        }
    }

    void ContinuationList::breakpoint(float y0, float y1, size_t line, TypedPtr location)
    {
        assert(!mBreakpoint);
        bool set = mContext.getBreakpoint(location.ptr(), line);
        if (Breakpoint(y0, y1, &set))
            mContext.setBreakpoint(location.ptr(), line, set);
    }

    void ContinuationList::lazyBreakpoint(const Execution::State::Breakpoint &bp, size_t line, TypedPtr location)
    {
        mLineFeedback = bp.mLineFeedback;
        mBreakpoint = line;
        mContinuation = bp.mContinuation;
        mIsActive = bp.mActive;
        mStopCallback = bp.mStopCallback();
        mLocation = location;
    }

    void ContinuationList::marker()
    {
        mIsMarker = true;
    }

    void ContinuationList::flush(float y0, float y1)
    {
        if (mBreakpoint) {
            if (mLineFeedback) {
                if (!*mLineFeedback) {
                    *mLineFeedback = mBreakpoint;
                } else if (*mLineFeedback != mBreakpoint) {
                    if (ImGui::BeginErrorTooltip()) {
                        ImGui::Text("Conflicting Lines: " + std::to_string(*mLineFeedback) + " vs " + std::to_string(mBreakpoint));
                        ImGui::EndErrorTooltip();
                    }
                    ErrorMarker(y0, y1);
                }
            }
            bool set = mContext.getBreakpoint(mLocation.ptr(), mBreakpoint);
            if (Breakpoint(y0, y1, &set))
                mContext.setBreakpoint(mLocation.ptr(), mBreakpoint, set);
            mBreakpoint.reset();
            if (mContinuation) {
                if (*mContinuation)
                    DrawDebugMarker(0.5f * (y0 + y1));

                controls(*mContinuation, std::move(mStopCallback), mIsActive);
                mContinuation = nullptr;
                mStopCallback = {};
                mIsActive = false;
            }
        }
        if (mIsMarker) {
            mIsMarker = false;
            DrawDebugMarker(0.5f * (y0 + y1));
        }
    }

    void ContinuationList::controls(Debug::Continuation &continuation, Closure<void()> stopCallback, bool isActive)
    {
        ControlButton button = mButton;

        if ((continuation && isActive) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
            ImGui::PushID(&continuation);

            if (ImGui::InlineContextButton(IMGUI_ICON_STOP)) {
                button = ControlButton::STOP;
            }

            if (continuation && isActive) {
                if (ImGui::InlineContextButton(IMGUI_ICON_STEP)) {
                    button = ControlButton::STEPOVER;
                }

                if (ImGui::InlineContextButton(IMGUI_ICON_STEP "##1")) {
                    button = ControlButton::STEPINTO;
                }

                if (ImGui::InlineContextButton(IMGUI_ICON_PLAY)) {
                    button = ControlButton::PLAY;
                }
            } else {
                if (ImGui::InlineContextButton(IMGUI_ICON_PAUSE)) {
                    button = ControlButton::PAUSE;
                }
            }

            ImGui::PopID();
        }

        switch (button) {
        case ControlButton::PLAY:
            mContinuations.emplace_back(continuation, Debug::ContinuationMode::Continue, Closure<void()> {});
            break;
        case ControlButton::STEPOVER:
            mContinuations.emplace_back(continuation, Debug::ContinuationMode::Step, Closure<void()> {});
            break;
        case ControlButton::STEPINTO:
            mContinuations.emplace_back(continuation, Debug::ContinuationMode::StepInto, Closure<void()> {});
            break;
        case ControlButton::STOP:
            if (continuation.stop()) {
                mContinuations.emplace_back(continuation, Debug::ContinuationMode::Done, std::move(stopCallback));
            }
            break;
        case ControlButton::PAUSE:
            continuation.pause();
            break;
        case ControlButton::NONE:
            break;
        }
    }

}
}
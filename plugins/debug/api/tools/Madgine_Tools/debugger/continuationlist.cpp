#include "../debugtoolslib.h"

#include "continuationlist.h"

#include "Madgine/debug/continuation.h"

#include "Madgine_Tools/imguiicons.h"
#include "imgui/imgui.h"
#include "imgui/imguiaddons.h"

namespace Engine {
namespace Tools {

    ContinuationList::ContinuationList(ControlButton button)
        : mButton(button)
    {
    }

    ContinuationList::~ContinuationList()
    {
        for (auto &[continuation, mode, callback] : mContinuations) {
            if (callback)
                callback();
            continuation(mode);
        }
    }

    void ContinuationList::controls(Debug::Continuation &continuation, Closure<void()> stopCallback)
    {
        ControlButton button = mButton;
        
        if (continuation || ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
            ImGui::PushID(&continuation);            

            if (ImGui::InlineContextButton(IMGUI_ICON_STOP)) {
                button = ControlButton::STOP;
            }

            if (ImGui::InlineContextButton(IMGUI_ICON_STEP)) {
                button = ControlButton::STEPOVER;
            }

            if (ImGui::InlineContextButton(IMGUI_ICON_STEP "##1")) {
                button = ControlButton::STEPINTO;
            }

            if (ImGui::InlineContextButton(IMGUI_ICON_PLAY)) {
                button = ControlButton::PLAY;
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
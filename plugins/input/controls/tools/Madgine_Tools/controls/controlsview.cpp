#include "../controlstoolslib.h"

#include "controlsview.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Modules//keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

#include "Interfaces/input/inputevents.h"

#include "Madgine/app/application.h"

#include "Madgine/controls/controlsmanager.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::ControlsView, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::ControlsView)

METATABLE_BEGIN_BASE(Engine::Tools::ControlsView, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::ControlsView)

UNIQUECOMPONENT(Engine::Tools::ControlsView)

    namespace Engine
{
    namespace Tools {

        ControlsView::ControlsView(ImRoot &root)
            : VirtualUnit(root)
        {
        }

        std::string_view ControlsView::key() const
        {
            return "ControlsView";
        }

        bool ControlsView::init()
        {
            Engine::App::Application::getSingleton().getGlobalAPIComponent<Controls::ControlsManager>().addAxisEventListener(this);

            return VirtualUnit::init();
        }

        void ControlsView::render()
        {
            if (ImGui::Begin("Controls - Debug", &mVisible)) {



                const ImGuiStyle &Style = ImGui::GetStyle();
                ImDrawList *DrawList = ImGui::GetWindowDrawList();
                ImGuiWindow *window = ImGui::GetCurrentWindow();

                // prepare canvas
                const float avail = ImGui::GetContentRegionAvailWidth();
                const float dim = min(avail, 128.f);
                ImVec2 Canvas(dim, dim);

                ImRect bb(window->DC.CursorPos, window->DC.CursorPos + Canvas);
                ImGui::ItemSize(bb);
                if (ImGui::ItemAdd(bb, 0)) {

                    ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg, 1), true, Style.FrameRounding);

                    // background grid
                    for (int i = 0; i <= Canvas.x; i += (Canvas.x / 4)) {
                        DrawList->AddLine(
                            ImVec2(bb.Min.x + i, bb.Min.y),
                            ImVec2(bb.Min.x + i, bb.Max.y),
                            ImGui::GetColorU32(ImGuiCol_TextDisabled));
                    }
                    for (int i = 0; i <= Canvas.y; i += (Canvas.y / 4)) {
                        DrawList->AddLine(
                            ImVec2(bb.Min.x, bb.Min.y + i),
                            ImVec2(bb.Max.x, bb.Min.y + i),
                            ImGui::GetColorU32(ImGuiCol_TextDisabled));
                    }

                    DrawList->AddCircle(bb.Min + Canvas / 2, 0.5f * dim, ImGui::GetColorU32(ImGuiCol_TextDisabled), 36);

                    ImVec2 v { 0.4f * mLeftStickX + 0.5f, 0.4f * mLeftStickY + 0.5f};

                    ImVec2 pos = bb.Min + v * Canvas;

                    DrawList->AddLine(pos - ImVec2(5.0f, 0), pos + ImVec2(5.0f, 0), ImGui::GetColorU32(ImGuiCol_ButtonHovered), 2.0f);
                    DrawList->AddLine(pos - ImVec2(0, 5.0f), pos + ImVec2(0, 5.0f), ImGui::GetColorU32(ImGuiCol_ButtonHovered), 2.0f);

                    // restore cursor pos
                    ImGui::SetCursorScreenPos(ImVec2(bb.Min.x, bb.Max.y)); // :P
                }
            }
            ImGui::End();
        }

        bool ControlsView::onAxisEvent(const Input::AxisEventArgs &arg)
        {
            if (arg.mAxisType == Input::AxisEventArgs::LEFT) {
                mLeftStickX = arg.mAxis1;
                mLeftStickY = arg.mAxis2;
            }
            return false;
        }
    }
}

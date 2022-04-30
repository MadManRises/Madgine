#include "../clienttoolslib.h"

#include "gamepadviewer.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Interfaces/input/inputevents.h"

#include "../imgui/clientimroot.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::GamepadViewer, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::GamepadViewer)

METATABLE_BEGIN_BASE(Engine::Tools::GamepadViewer, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::GamepadViewer)

UNIQUECOMPONENT(Engine::Tools::GamepadViewer)

namespace Engine {
namespace Tools {

    GamepadViewer::GamepadViewer(ImRoot &root)
        : VirtualUnit(root)
    {
    }

    std::string_view GamepadViewer::key() const
    {
        return "GamepadViewer";
    }

    void GamepadViewer::render()
    {
        if (ImGui::Begin("GamepadViewer", &mVisible)) {

            const ImGuiStyle &Style = ImGui::GetStyle();
            ImDrawList *DrawList = ImGui::GetWindowDrawList();
            ImGuiWindow *window = ImGui::GetCurrentWindow();

            ClientImRoot &root = static_cast<ClientImRoot &>(mRoot);

            // prepare canvas
            const ImVec2 avail = ImGui::GetContentRegionAvail();
            const float dimX = (avail.x - 20) / 2;
            const float dimY = avail.y;
            const float dim = max(20.0f, min(128.0f, min(dimX, dimY)));
            ImVec2 Canvas(dim, dim);

            for (Vector2 axis : { root.mLeftControllerStick, root.mRightControllerStick }) {

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

                    ImVec2 v { 0.4f * axis.x + 0.5f, 0.4f * axis.y + 0.5f };

                    ImVec2 pos = bb.Min + v * Canvas;

                    DrawList->AddLine(pos - ImVec2(5.0f, 0), pos + ImVec2(5.0f, 0), ImGui::GetColorU32(ImGuiCol_ButtonHovered), 2.0f);
                    DrawList->AddLine(pos - ImVec2(0, 5.0f), pos + ImVec2(0, 5.0f), ImGui::GetColorU32(ImGuiCol_ButtonHovered), 2.0f);

                    // restore cursor pos
                    ImGui::SetCursorScreenPos(ImVec2(bb.Max.x + 20, bb.Min.y)); // :P
                }
            }
            ImGui::Text("%d", root.mDPadState);

            ImGuiIO &io = ImGui::GetIO();

#define BUTTON(name)                   \
    if (io.KeysDown[Input::Key::name]) \
    ImGui::Text(#name)
            BUTTON(GP_A);
            BUTTON(GP_B);
            BUTTON(GP_X);
            BUTTON(GP_Y);
            BUTTON(GP_LB);
            BUTTON(GP_RB);
            BUTTON(GP_LSB);
            BUTTON(GP_RSB);
            BUTTON(GP_B1);
            BUTTON(GP_B2);
        }
        ImGui::End();
    }

}
}
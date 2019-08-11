#include "../toolslib.h"

#include "guieditor.h"

#include "../imgui/imgui.h"
#include "../renderer/imguiroot.h"

#include "../renderer/imguiaddons.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/math/bounds.h"
#include "Modules/reflection/classname.h"

#include "gui/guisystem.h"
#include "gui/widgets/toplevelwindow.h"
#include "gui/widgets/widget.h"

#include "Interfaces/window/windowapi.h"

UNIQUECOMPONENT(Engine::Tools::GuiEditor);

namespace Engine {
namespace Tools {

    GuiEditor::GuiEditor(ImGuiRoot &root)
        : Tool<GuiEditor>(root)
    {
    }

    bool GuiEditor::init()
    {
        mWindow = mRoot.getGlobalAPIComponent<GUI::GUISystem>(false).topLevelWindows().front().get();

        return ToolBase::init();
    }

    void GuiEditor::render()
    {

        if (ImGui::Begin("GuiEditor", &mVisible)) {

            ImDrawList *background = ImGui::GetBackgroundDrawList();

            auto [screenPos, screenSize] = mWindow->getAvailableScreenSpace();
            screenPos += Vector3 {
                static_cast<float>(mWindow->window()->renderX()), static_cast<float>(mWindow->window()->renderY()), 0.0f
            };

            ImGuiIO &io = ImGui::GetIO();

            Vector2 mouse = ImGui::GetMousePos();
            Vector2 dragDistance = mouse - io.MouseClickedPos[0];

            GUI::Widget *hoveredWidget = mWindow->hoveredWidget();
            WidgetSettings *hoveredSettings = nullptr;

            if (!io.WantCaptureMouse) {
                if (hoveredWidget) {
                    hoveredSettings = static_cast<WidgetSettings *>(hoveredWidget->userData());
                    if (!hoveredSettings) {
                        hoveredSettings = &mSettings.emplace_back(hoveredWidget);
                    }

                    if (!mDragging) {
                        Vector3 size = hoveredWidget->getAbsoluteSize() * screenSize;
                        Vector3 pos = hoveredWidget->getAbsolutePosition() * screenSize + screenPos;

                        Math::Bounds bounds(pos.x, pos.y + size.y, pos.x + size.x, pos.y);

                        background->AddRect(bounds.topLeft(), bounds.bottomRight(), IM_COL32(127, 127, 127, 255));
                    }
                }
                if (io.MouseReleased[0] && !mDragging) {
                    mSelected = hoveredSettings;
                }
            }

            if (mSelected) {
                GUI::Widget *selectedWidget = mSelected->widget();

                Vector3 absoluteSize = selectedWidget->getAbsoluteSize() * screenSize;
                Vector3 absolutePos = selectedWidget->getAbsolutePosition() * screenSize + screenPos;

                Math::Bounds bounds(absolutePos.x, absolutePos.y + absoluteSize.y, absolutePos.x + absoluteSize.x, absolutePos.y);

                background->AddRect(bounds.topLeft(), bounds.bottomRight(), IM_COL32(255, 255, 255, 255));

                enum ResizeMode {
                    PROPORTIONAL,
                    RELATIVE,
                    ABSOLUTE
                };

                ResizeMode resizeMode = PROPORTIONAL;
                ImU32 resizeColor = IM_COL32(0, 255, 255, 255);
                if (io.KeyCtrl) {
                    resizeMode = RELATIVE;
                    resizeColor = IM_COL32(0, 255, 0, 255);
                }
                if (io.KeyAlt) {
                    resizeMode = ABSOLUTE;
                    resizeColor = IM_COL32(0, 0, 255, 255);
                }

                if (!io.WantCaptureMouse) {

                    bool rightBorder = false, leftBorder = false, topBorder = false, bottomBorder = false;
                    constexpr float borderSize = 10.0f;

                    if (!mDragging) {
                        leftBorder = abs(mouse.x - bounds.left()) < borderSize;
                        rightBorder = abs(mouse.x - bounds.right()) < borderSize;
                        topBorder = abs(mouse.y - bounds.top()) < borderSize;
                        bottomBorder = abs(mouse.y - bounds.bottom()) < borderSize;

                        if (mSelected->aspectRatio()) {
                            if (topBorder || leftBorder) {
                                leftBorder = !rightBorder;
                                topBorder = !bottomBorder;
                            }
                            if (bottomBorder || rightBorder) {
                                rightBorder = !leftBorder;
                                bottomBorder = !topBorder;
                            }
                        }
                    }

                    constexpr float thickness = 4.0f;

                    //TODO
                    if (leftBorder || mDraggingLeft)
                        background->AddLine(bounds.topLeft(), bounds.bottomLeft(), resizeColor, thickness);
                    if (rightBorder || mDraggingRight)
                        background->AddLine(bounds.topRight(), bounds.bottomRight(), resizeColor, thickness);
                    if (topBorder || mDraggingTop)
                        background->AddLine(bounds.topLeft(), bounds.topRight(), resizeColor, thickness);
                    if (bottomBorder || mDraggingBottom)
                        background->AddLine(bounds.bottomLeft(), bounds.bottomRight(), resizeColor, thickness);

                if (io.MouseClicked[0] && selectedWidget->containsPoint(mouse, screenSize, screenPos, borderSize)) {
                    mMouseDown = true;
                    mDraggingLeft = leftBorder;
                    mDraggingRight = rightBorder;
                    mDraggingTop = topBorder;
                    mDraggingBottom = bottomBorder;
                }

                    if (mMouseDown && dragDistance.length() >= io.MouseDragThreshold && !mDragging) {
                        mSelected->saveGeometry();
                        mDragging = true;
                    }
                }

                if (mDragging) {

                    auto [pos, size] = mSelected->savedGeometry();

                    float mod[2][2] { { 0.0f, 0.0f }, { 0.0f, 0.0f } };
                    Vector2 relDragDistance = dragDistance / (size * screenSize).xy();

                    switch (resizeMode) {
                    case PROPORTIONAL:
                        mod[0][0] = relDragDistance.x;
                        mod[1][0] = relDragDistance.y;
                        break;
                    case ABSOLUTE:
                        mod[0][1] = dragDistance.x;
                        mod[1][1] = dragDistance.y;
                        break;
                    case RELATIVE:
                        break;
                    }

                    if (!mDraggingLeft && !mDraggingRight && !mDraggingTop && !mDraggingBottom) {
                        pos[0][0] += mod[0][0] * size[0][0];
                        pos[0][1] += mod[0][0] * size[0][1];
                        pos[0][2] += mod[0][0] * size[0][2] + mod[0][1];
                        pos[1][0] += mod[1][0] * size[1][0];
                        pos[1][1] += mod[1][0] * size[1][1];
                        pos[1][2] += mod[1][0] * size[1][2] + mod[1][1];
                    } else {

                        if (mDraggingLeft) {
                            mod[0][0] *= -1.0f;
                            mod[0][1] *= -1.0f;
                        }
                        if (mDraggingTop) {
                            mod[1][0] *= -1.0f;
                            mod[1][1] *= -1.0f;
                        }

                        std::optional<float> aspectRatio = mSelected->aspectRatio();
                        if (aspectRatio) {
                            mod[1][0] = mod[0][0];
                            mod[1][1] = mod[0][1] / *aspectRatio;
                        }

                        if (mDraggingLeft || mDraggingRight) {
                            if (mDraggingLeft) {
                                pos[0][0] -= mod[0][0] * size[0][0];
                                pos[0][1] -= mod[0][0] * size[0][1];
                                pos[0][2] -= mod[0][0] * size[0][2] + mod[0][1];
                            }

                            size[0][0] += mod[0][0] * size[0][0];
                            size[0][1] += mod[0][0] * size[0][1];
                            size[0][2] += mod[0][0] * size[0][2] + mod[0][1];
                        }

                        if (mDraggingTop || mDraggingBottom) {
                            if (mDraggingTop) {
                                pos[1][0] -= mod[1][0] * size[1][0];
                                pos[1][1] -= mod[1][0] * size[1][1];
                                pos[1][2] -= mod[1][0] * size[1][2] + mod[1][1];
                            }

                            size[1][0] += mod[1][0] * size[1][0];
                            size[1][1] += mod[1][0] * size[1][1];
                            size[1][2] += mod[1][0] * size[1][2] + mod[1][1];
                        }
                    }

                    mSelected->setSize(size);
                    mSelected->setPos(pos);

                    if (io.MouseReleased[0]) {
                        mSelected->applyGeometry();
                        mDragging = false;
                    }
                }

                if (io.MouseReleased[0]) {
                    mMouseDown = false;
                    mDraggingLeft = false;
                    mDraggingRight = false;
                    mDraggingTop = false;
                    mDraggingBottom = false;
                }
            }

            if (mSelected) {
                ImGui::Text("Selected");
                mSelected->render();
            }
            if (hoveredSettings && !mDragging && hoveredSettings != mSelected) {
                ImGui::Text("Hovered");
                hoveredSettings->render();
            }

			io.WantCaptureMouse = true;
        }
        ImGui::End();
    }

    void GuiEditor::renderMenu()
    {
        if (ImGui::BeginMenu("Layouts")) {
            for (GUI::Widget *w : mWindow->widgets()) {
                if (ImGui::MenuItem(w->key(), nullptr, w->isVisible())) {
                    mWindow->swapCurrentRoot(w);
                }
            }
            ImGui::EndMenu();
        }
    }

    void GuiEditor::update()
    {

        ToolBase::update();
    }

    const char *GuiEditor::key() const
    {
        return "GuiEditor";
    }

}
}

METATABLE_BEGIN(Engine::Tools::GuiEditor)
METATABLE_END(Engine::Tools::GuiEditor)

RegisterType(Engine::Tools::GuiEditor);
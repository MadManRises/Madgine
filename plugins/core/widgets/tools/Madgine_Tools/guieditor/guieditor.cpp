#include "../widgetstoolslib.h"

#include "guieditor.h"

#include "Madgine_Tools/imgui/clientimroot.h"
#include "imgui/imgui.h"

#include "imgui/imguiaddons.h"

#include "inspector/inspector.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/math/bounds.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Madgine/window/mainwindow.h"
#include "Madgine/widgets/widget.h"

#include "Interfaces/window/windowapi.h"

#include "project/projectmanager.h"

#include "Meta/serialize/streams/serializestream.h"

#include "Meta/serialize/formatter/xmlformatter.h"

#include "serialize/filesystem/filesystemlib.h"
#include "serialize/filesystem/filemanager.h"

#include "Madgine/widgets/widgetclass.h"

#include "Madgine/widgets/widgetmanager.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/serialize/streams/serializestreamdata.h"

UNIQUECOMPONENT(Engine::Tools::GuiEditor);

METATABLE_BEGIN_BASE(Engine::Tools::GuiEditor, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::GuiEditor)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::GuiEditor, Engine::Tools::ToolBase)
FIELD(mHierarchyVisible)
SERIALIZETABLE_END(Engine::Tools::GuiEditor)

namespace Engine {
namespace Tools {

    GuiEditor::GuiEditor(ImRoot &root)
        : Tool<GuiEditor>(root)
    {
    }

    bool GuiEditor::init()
    {
        mWindow = &static_cast<const ClientImRoot &>(mRoot).window();
        mWidgetManager = &mWindow->getWindowComponent<Widgets::WidgetManager>();

#if ENABLE_PLUGINS
        getTool<ProjectManager>().mProjectChanged.connect([this]() {
            loadLayout();
        });

        mWindow->frameLoop().queue([this]() {
            loadLayout();
        }, Threading::TaskMask::DEFAULT);
#endif

        return ToolBase::init();
    }

    void GuiEditor::render()
    {
        Widgets::WidgetBase *hoveredWidget = nullptr;
        if (mHierarchyVisible)
            renderHierarchy(&hoveredWidget);
        renderSelection(hoveredWidget);
    }

    void GuiEditor::renderMenu()
    {
        if (ImGui::BeginMenu("Layouts")) {
            for (Widgets::WidgetBase *w : mWidgetManager->widgets()) {
                if (ImGui::MenuItem(w->key().c_str(), nullptr, w->mVisible)) {
                    mWidgetManager->swapCurrentRoot(w);
                }
            }
            ImGui::Separator();
            if (ImGui::Button("Create Layout")) {
                mWidgetManager->createTopLevel<>("Unnamed");
            }
            ImGui::EndMenu();
        }

        if (mVisible) {

            if (ImGui::BeginMenu("GuiEditor")) {

                ImGui::MenuItem("Hierarchy", nullptr, &mHierarchyVisible);

                if (ImGui::MenuItem("Save Layout")) {
                    saveLayout();
                }

                if (ImGui::MenuItem("Load Layout")) {
                    loadLayout();
                }

                ImGui::EndMenu();
            }
        }
    }

    void GuiEditor::update()
    {
        ToolBase::update();
    }

    std::string_view GuiEditor::key() const
    {
        return "GuiEditor";
    }

    void GuiEditor::saveLayout()
    {
        Filesystem::Path filePath = getTool<ProjectManager>().projectRoot() / "data" / "default.layout";

        auto buf = std::make_unique<std::filebuf>();
        buf->open(filePath.str(), std::ios::out);
        Serialize::SerializeOutStream out { std::move(buf), std::make_unique<Serialize::SerializeStreamData>(std::make_unique<Serialize::XMLFormatter>()) };

        mWindow->writeState(out);
    }

    void GuiEditor::loadLayout()
    {
        ProjectManager &project = getTool<ProjectManager>();
        const Filesystem::Path &root = project.projectRoot();
        const std::string &config = project.config();

        if (!config.empty()) {

            Filesystem::Path filePath = root / "data" / (config + ".layout");

            Filesystem::FileManager file("Layout");
            Serialize::SerializeInStream in = file.openRead(filePath, std::make_unique<Serialize::XMLFormatter>());
            if (in) {
                Serialize::StreamResult result = mWindow->readState(in, nullptr, {}, Serialize::StateTransmissionFlags_ApplyMap);
                if (result.mState != Serialize::StreamState::OK) {
                    LOG_ERROR("Failed loading '" << filePath << "' with following Error: "
                                                 << "\n"
                                                 << *result.mError);
                }
            }
        }
    }

    void GuiEditor::renderSelection(Widgets::WidgetBase *hoveredWidget)
    {
        constexpr float borderSize = 10.0f;

        if (ImGui::Begin("GuiEditor", &mVisible)) {

            ImDrawList *background = ImGui::GetBackgroundDrawList(ImGui::GetMainViewport());

            Rect2i screenSpace = mWidgetManager->getClientSpace();

            InterfacesVector pos = mWidgetManager->window().osWindow()->renderPos();
            Vector3i windowPos = Vector3i{
                pos.x, pos.y, 0
            };

            ImGuiIO &io = ImGui::GetIO();

            Vector2 mouse = ImGui::GetMousePos();
            Vector2 dragDistance = mouse - io.MouseClickedPos[0];

            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
                screenSpace += mWidgetManager->getScreenSpace().mTopLeft;

            bool acceptHover = (hoveredWidget != nullptr || !io.WantCaptureMouse);

            if (mSelected) {
                Widgets::WidgetBase *selectedWidget = mSelected->widget();

                Vector3 absoluteSize = selectedWidget->getAbsoluteSize() * Vector3 { Vector2 { screenSpace.mSize }, 1.0f };
                Vector3 absolutePos = selectedWidget->getAbsolutePosition() * Vector3 { Vector2 { screenSpace.mSize }, 1.0f } + Vector3 { Vector2 { screenSpace.mTopLeft }, 0.0f };

                Math::Bounds bounds(absolutePos.x, absolutePos.y + absoluteSize.y, absolutePos.x + absoluteSize.x, absolutePos.y);

                background->AddRect(bounds.topLeft() / io.DisplayFramebufferScale, bounds.bottomRight() / io.DisplayFramebufferScale, IM_COL32(255, 255, 255, 255));

                ImU32 resizeColor = IM_COL32(0, 255, 255, 255);
                if (io.KeyShift) {
                    resizeColor = IM_COL32(0, 255, 0, 255);
                }

                if (!io.WantCaptureMouse) {

                    bool rightBorder = false, leftBorder = false, topBorder = false, bottomBorder = false;

                    if (!mDragging && selectedWidget->containsPoint(mouse, screenSpace, borderSize)) {
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

                        acceptHover &= (!rightBorder && !leftBorder && !topBorder && !bottomBorder);
                    }

                    constexpr float thickness = 4.0f;

                    //TODO
                    if (leftBorder || mDraggingLeft)
                        background->AddLine(bounds.topLeft() / io.DisplayFramebufferScale, bounds.bottomLeft() / io.DisplayFramebufferScale, resizeColor, thickness);
                    if (rightBorder || mDraggingRight)
                        background->AddLine(bounds.topRight() / io.DisplayFramebufferScale, bounds.bottomRight() / io.DisplayFramebufferScale, resizeColor, thickness);
                    if (topBorder || mDraggingTop)
                        background->AddLine(bounds.topLeft() / io.DisplayFramebufferScale, bounds.topRight() / io.DisplayFramebufferScale, resizeColor, thickness);
                    if (bottomBorder || mDraggingBottom)
                        background->AddLine(bounds.bottomLeft() / io.DisplayFramebufferScale, bounds.bottomRight() / io.DisplayFramebufferScale, resizeColor, thickness);

                    if (io.MouseClicked[0] && selectedWidget->containsPoint(mouse, screenSpace, borderSize)) {
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
            }

            if (!hoveredWidget)
                hoveredWidget = mWidgetManager->hoveredWidget();
            WidgetSettings *hoveredSettings = nullptr;

            if (acceptHover) {
                if (hoveredWidget) {
                    hoveredSettings = static_cast<WidgetSettings *>(hoveredWidget->userData());
                    if (!hoveredSettings) {
                        hoveredSettings = &mSettings.emplace_back(hoveredWidget, getTool<Inspector>());
                    }

                    if (!mDragging) {
                        Vector3 size = hoveredWidget->getAbsoluteSize() * Vector3 { Vector2 { screenSpace.mSize }, 1.0f };
                        Vector3 pos = hoveredWidget->getAbsolutePosition() * Vector3 { Vector2 { screenSpace.mSize }, 1.0f } + Vector3 { Vector2 { screenSpace.mTopLeft }, 0.0f };

                        Math::Bounds bounds(pos.x, pos.y + size.y, pos.x + size.x, pos.y);

                        background->AddRect(bounds.topLeft() / io.DisplayFramebufferScale, bounds.bottomRight() / io.DisplayFramebufferScale, IM_COL32(127, 127, 127, 255));
                    }
                }
                if (io.MouseReleased[0] && !mDragging) {
                    mSelected = hoveredSettings;
                }
            }

            if (mSelected) {

                enum ResizeMode {
                    RELATIVE,
                    ABSOLUTE
                };

                ResizeMode resizeMode = RELATIVE;
                if (io.KeyShift) {
                    resizeMode = ABSOLUTE;
                }

                if (mDragging) {

                    auto [pos, size] = mSelected->savedGeometry();

                    Matrix3 parentSize = mSelected->widget()->getParent() ? mSelected->widget()->getParent()->getAbsoluteSize() : Matrix3::IDENTITY;

                    Vector2 relDragDistance = dragDistance / (parentSize * Vector3 { Vector2 { screenSpace.mSize }, 1.0f }).xy();

                    Matrix3 dragDistanceSize;

                    switch (resizeMode) {
                    case RELATIVE:
                        dragDistanceSize = Matrix3 {
                            relDragDistance.x, 0, 0,
                            0, relDragDistance.y, 0,
                            0, 0, 0
                        };
                        break;
                    case ABSOLUTE:
                        dragDistanceSize = Matrix3 {
                            0, 0, dragDistance.x / parentSize[2][2],
                            0, 0, dragDistance.y / parentSize[2][2],
                            0, 0, 0
                        };
                        break;
                    }

                    if (!mDraggingLeft && !mDraggingRight && !mDraggingTop && !mDraggingBottom) {
                        pos += dragDistanceSize;
                    } else {
                        Matrix3 dragDistancePos { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
                        if (!mDraggingLeft && !mDraggingRight) {
                            dragDistanceSize[0][0] = 0.0f;
                            dragDistanceSize[0][2] = 0.0f;
                            dragDistancePos[0][0] = 0.0f;
                            dragDistancePos[0][2] = 0.0f;
                        } else if (mDraggingLeft) {
                            dragDistancePos[0][0] = dragDistanceSize[0][0];
                            dragDistancePos[0][2] = dragDistanceSize[0][2];
                            dragDistanceSize[0][0] *= -1.0f;
                            dragDistanceSize[0][2] *= -1.0f;
                        }
                        if (!mDraggingTop && !mDraggingBottom) {
                            dragDistanceSize[1][1] = 0.0f;
                            dragDistanceSize[1][2] = 0.0f;
                            dragDistancePos[1][1] = 0.0f;
                            dragDistancePos[1][2] = 0.0f;
                        } else if (mDraggingTop) {
                            if (mSelected->aspectRatio()) {
                                dragDistancePos[1][0] = -dragDistanceSize[0][0];
                                dragDistancePos[1][1] = 0.0f;
                                dragDistancePos[1][2] = -dragDistanceSize[0][2];
                            } else {
                                dragDistancePos[1][1] = dragDistanceSize[1][1];
                                dragDistancePos[1][2] = dragDistanceSize[1][2];
                            }
                            dragDistanceSize[1][1] *= -1.0f;
                            dragDistanceSize[1][2] *= -1.0f;
                        }

                        pos += dragDistancePos;
                        size += dragDistanceSize;
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
            /*if (hoveredSettings && !mDragging && hoveredSettings != mSelected) {
                ImGui::Text("Hovered");
                hoveredSettings->render();
            }*/

            //io.WantCaptureMouse = true;
        }
        ImGui::End();
    }

    bool GuiEditor::drawWidget(Widgets::WidgetBase *w, Widgets::WidgetBase **hoveredWidget)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
        if (w->children().empty())
            flags |= ImGuiTreeNodeFlags_Leaf;
        if (mSelected && mSelected->widget() == w)
            flags |= ImGuiTreeNodeFlags_Selected;

        bool open = ImGui::TreeNodeEx(w->getName().c_str(), flags);

        ImGui::DraggableValueTypeSource(w->getName(), w);
        if (ImGui::BeginDragDropTarget()) {
            Widgets::WidgetBase *newChild = nullptr;
            if (ImGui::AcceptDraggableValueType(newChild)) {
                newChild->setParent(w);
            }
            ImGui::EndDragDropTarget();
            if (newChild) {
                if (open)
                    ImGui::TreePop();
                return false;
            }
        }

        if (open) {
            for (Widgets::WidgetBase *child : w->children()) {
                if (!drawWidget(child, hoveredWidget)) {
                    break;
                }
            }

            ImGui::TreePop();
        }

        if (hoveredWidget && !*hoveredWidget) {
            if (ImGui::IsItemHovered()) {
                *hoveredWidget = w;
            }
        }
        return true;
    }

    void GuiEditor::renderHierarchy(Widgets::WidgetBase **hoveredWidget)
    {
        if (ImGui::Begin("GuiEditor - Hierarchy", &mHierarchyVisible)) {
            Widgets::WidgetBase *root = mWidgetManager->currentRoot();
            if (root) {
                if (ImGui::BeginPopup("WidgetSelector")) {
                    for (Widgets::WidgetClass c : Widgets::WidgetClass::values()) {
                        if (ImGui::Selectable(std::string { c.toString() }.c_str())) {
                            root->createChild("unnamed", c);
                            ImGui::CloseCurrentPopup();
                        }
                    }
                    ImGui::EndPopup();
                }
                if (ImGui::Button("+ New Widget"))
                    ImGui::OpenPopup("WidgetSelector");
                drawWidget(root, hoveredWidget);
            } else {
                ImGui::Text("Please select a root window under 'Layout' in the menu bar.");
            }
        }
        ImGui::End();
    }

}
}


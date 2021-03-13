#include "../nodegraphtoolslib.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imguiaddons.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Madgine/nodegraph/nodecollector.h"
#include "nodegrapheditor.h"

#include "NodeEditor/ax/Widgets.h"
#include "NodeEditor/imgui_node_editor.h"

#include "Interfaces/filesystem/api.h"

#include "Madgine/nodegraph/nodebase.h"

#include "inspector/inspector.h"

#include "Madgine/nodegraph/nodeinterpreter.h"

UNIQUECOMPONENT(Engine::Tools::NodeGraphEditor);

METATABLE_BEGIN_BASE(Engine::Tools::NodeGraphEditor, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::NodeGraphEditor)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::NodeGraphEditor, Engine::Tools::ToolBase)
FIELD(mHierarchyVisible)
FIELD(mNodeDetailsVisible)
SERIALIZETABLE_END(Engine::Tools::NodeGraphEditor)

static const int sPinIconSize = 24;

namespace Engine {
namespace Tools {

    void ShowLabel(std::string_view label, ImColor color = { 0.0f, 0.0f, 0.0f })
    {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
        auto size = ImGui::CalcTextSize(label.data(), label.data() + label.size());

        auto padding = ImGui::GetStyle().FramePadding;
        auto spacing = ImGui::GetStyle().ItemSpacing;

        ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

        auto rectMin = ImGui::GetCursorScreenPos() - padding;
        auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

        auto drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
        ImGui::TextUnformatted(label.data(), label.data() + label.size());
    };

    void DataPinIcon(ValueTypeDesc type, bool connected = false)
    {
        ax::Widgets::Icon(ImVec2(sPinIconSize, sPinIconSize), ax::Widgets::IconType::Circle, connected, ImColor { 0, 255, 0, 255 }, ImColor(32, 32, 32, 255));
        ImVec2 align { 0.5f, 0.5f };
        ImVec2 pos = ImGui::GetItemRectMin() + align * ImGui::GetItemRectSize();
        ed::PinPivotRect(pos, pos);
    }

    void DataInstancePinIcon(ValueTypeDesc type, bool connected = false)
    {
        ax::Widgets::Icon(ImVec2(sPinIconSize, sPinIconSize), ax::Widgets::IconType::Square, connected, ImColor { 0, 255, 0, 255 }, ImColor(32, 32, 32, 255));
        ImVec2 align { 0.5f, 0.5f };
        ImVec2 pos = ImGui::GetItemRectMin() + align * ImGui::GetItemRectSize();
        ed::PinPivotRect(pos, pos);
    }

    void FlowPinIcon()
    {
        ax::Widgets::Icon(ImVec2(sPinIconSize, sPinIconSize), ax::Widgets::IconType::Flow, false, ImColor { 255, 0, 0, 255 }, ImColor(32, 32, 32, 255));
        ImVec2 align { 0.5f, 0.5f };
        ImVec2 pos = ImGui::GetItemRectMin() + align * ImGui::GetItemRectSize();
        ed::PinPivotRect(pos, pos);
    }

    void FlowOutPin(const char *name, uint32_t nodeId, uint32_t pinId)
    {
        ed::BeginPin(6000 * nodeId + NodeGraph::NodeBase::flowOutId(pinId), ed::PinKind::Output);
        if (name) {
            ImGui::Text("%s", name);
            ImGui::SameLine();
        }
        FlowPinIcon();
        ed::EndPin();
    }

    void FlowInPin(const char *name, uint32_t nodeId, uint32_t pinId)
    {
        ed::BeginPin(6000 * nodeId + NodeGraph::NodeBase::flowInId(pinId), ed::PinKind::Input);
        FlowPinIcon();
        if (name) {
            ImGui::SameLine();
            ImGui::Text("%s", name);
        }
        ed::EndPin();
    }

    bool DataOutPin(const char *name, size_t nodeId, size_t pinId, ValueTypeDesc type, const NodeGraph::TargetPin &target)
    {
        ed::BeginPin(6000 * nodeId + NodeGraph::NodeBase::dataOutId(pinId), ed::PinKind::Output);
        if (name) {
            ImGui::Text("%s", name);
            ImGui::SameLine();
        }
        DataPinIcon(type, bool(target));
        ed::EndPin();
        return ImGui::IsItemHovered();
    }

    bool DataInPin(const char *name, size_t nodeId, size_t pinId, ExtendedValueTypeDesc type, const NodeGraph::TargetPin &source)
    {
        uintptr_t id = 6000 * nodeId + NodeGraph::NodeBase::dataInId(pinId);
        ed::BeginPin(id, ed::PinKind::Input);
        DataPinIcon(type, bool(source));
        if (name) {
            ImGui::SameLine();
            ImGui::Text("%s", name);
        }
        ed::EndPin();
        return ImGui::IsItemHovered();
    }

    bool DataProviderPin(const char *name, size_t nodeId, size_t pinId, ValueTypeDesc type)
    {
        ed::BeginPin(6000 * nodeId + NodeGraph::NodeBase::dataProviderId(pinId), ed::PinKind::Output);
        if (name) {
            ImGui::Text("%s", name);
            ImGui::SameLine();
        }
        DataInstancePinIcon(type);
        ed::EndPin();
        return ImGui::IsItemHovered();
    }

    bool DataReceiverPin(const char *name, size_t nodeId, size_t pinId, ExtendedValueTypeDesc type)
    {
        uintptr_t id = 6000 * nodeId + NodeGraph::NodeBase::dataReceiverId(pinId);
        ed::BeginPin(id, ed::PinKind::Input);
        DataInstancePinIcon(type);
        if (name) {
            ImGui::SameLine();
            ImGui::Text("%s", name);
        }
        ed::EndPin();
        return ImGui::IsItemHovered();
    }

    void HoverPin(ExtendedValueTypeDesc type)
    {
        if (type.mType.isRegular()) {
            ImVec2 cursor = ImGui::GetCursorPos();
            ImGui::SetCursorScreenPos(ImGui::GetMousePos());
            ShowLabel(type.toString());
            ImGui::SetCursorPos(cursor);
        }
    }

    NodeGraphEditor::NodeGraphEditor(ImRoot &root)
        : Tool<NodeGraphEditor>(root)
    {
    }

    bool NodeGraphEditor::init()
    {

        return ToolBase::init();
    }

    void NodeGraphEditor::finalize()
    {
        if (mEditor) {
            ed::DestroyEditor(mEditor);
            mEditor = nullptr;
        }

        ToolBase::finalize();
    }

    void NodeGraphEditor::render()
    {
        if (!mGraph)
            ImGui::PushDisabled();

        std::string fileName;
        if (mGraph) {
            fileName = mGraph.resource()->path().filename().str();

            std::string pivot = "###";

            if (mIsDirty)
                pivot = "*" + pivot;

            fileName = fileName + pivot + fileName;
        } else {
            fileName = "Node graph";
        }

        if (ImGui::Begin(fileName.c_str())) {

            if (mGraph) {

                ImVec2 topLeftScreen = ImGui::GetCursorScreenPos();

                //Probably set this in the implementation to the exact Canvas-View area
                ImVec2 oldViewportPos = ImGui::GetCurrentContext()->MouseViewport->Pos;
                ImVec2 oldViewportSize = ImGui::GetCurrentContext()->MouseViewport->Size;

                ImGui::GetCurrentContext()->MouseViewport->Pos = { -10000, -10000 };
                ImGui::GetCurrentContext()->MouseViewport->Size = { 20000, 20000 };

                ed::SetCurrentEditor(mEditor);

                std::optional<ExtendedValueTypeDesc> hoveredPin;

                ed::Begin("Node editor");

                auto alpha = ImGui::GetStyle().Alpha;

                ed::PushStyleVar(ed::StyleVar_NodePadding, { 0, 0, 0, 0 });
                ImVec2 specialPosition = ed::ScreenToCanvas(topLeftScreen);
                ed::SetNodePosition(std::numeric_limits<int>::max() - 1, { floorf(specialPosition.x), floorf(specialPosition.y) });
                ed::BeginNode(std::numeric_limits<int>::max() - 1);
                ImGui::Dummy({ sPinIconSize, 10 });
                uint32_t pinId = 0;
                for (NodeGraph::FlowOutPinPrototype &flowPin : mGraph->mFlowInPins) {
                    FlowOutPin(nullptr, 0, pinId);
                    ++pinId;
                }
                if (mDragPin && mDragPin->mDir == NodeGraph::PinDir::In && mDragPin->mType == NodeGraph::PinType::Flow) {
                    FlowOutPin(nullptr, 0, pinId);
                }
                pinId = 0;
                for (NodeGraph::DataOutPinPrototype &dataPin : mGraph->mDataReceiverPins) {
                    ExtendedValueTypeDesc type = mGraph->node(dataPin.mTarget.mNode)->dataInType(dataPin.mTarget.mIndex);
                    if (DataOutPin(nullptr, 0, pinId, type, dataPin.mTarget))
                        hoveredPin = type;
                    ++pinId;
                }
                if (mDragPin && mDragPin->mDir == NodeGraph::PinDir::In && mDragPin->mType == NodeGraph::PinType::DataInstance) {
                    DataOutPin(nullptr, 0, pinId, *mDragType, {});
                }
                pinId = 0;
                for (NodeGraph::DataProviderPinPrototype &dataPin : mGraph->mDataInPins) {
                    if (DataProviderPin(nullptr, 0, pinId, dataPin.mType))
                        hoveredPin = dataPin.mType;
                    ++pinId;
                }
                if (mDragPin && mDragPin->mDir == NodeGraph::PinDir::In && mDragPin->mType == NodeGraph::PinType::Data) {
                    DataProviderPin(nullptr, 0, pinId, *mDragType);
                }
                ed::EndNode();

                specialPosition = ed::ScreenToCanvas({ topLeftScreen.x + ed::GetScreenSize().x - sPinIconSize / ed::GetCurrentZoom(), topLeftScreen.y });
                ed::SetNodePosition(std::numeric_limits<int>::max() - 2, { floorf(specialPosition.x), floorf(specialPosition.y) });
                ed::BeginNode(std::numeric_limits<int>::max() - 2);
                ImGui::Dummy({ sPinIconSize, 10 });
                for (pinId = 0; pinId < mGraph->mFlowOutPinCount; ++pinId) {
                    FlowInPin(nullptr, 0, pinId);
                }
                if (mDragPin && mDragPin->mDir == NodeGraph::PinDir::Out && mDragPin->mType == NodeGraph::PinType::Flow) {
                    FlowInPin(nullptr, 0, pinId);
                }
                pinId = 0;
                for (NodeGraph::DataInPinPrototype &dataPin : mGraph->mDataProviderPins) {
                    assert(dataPin.mSource && dataPin.mSource.mNode);
                    ExtendedValueTypeDesc type = mGraph->node(dataPin.mSource.mNode)->dataProviderType(dataPin.mSource.mIndex);

                    if (DataInPin(nullptr, 0, pinId, type, dataPin.mSource))
                        hoveredPin = type;
                    ++pinId;
                }
                if (mDragPin && mDragPin->mDir == NodeGraph::PinDir::Out && mDragPin->mType == NodeGraph::PinType::DataInstance) {
                    DataInPin(nullptr, 0, pinId, *mDragType, {});
                }
                pinId = 0;
                for (NodeGraph::DataReceiverPinPrototype &dataPin : mGraph->mDataOutPins) {
                    if (DataReceiverPin(nullptr, 0, pinId, dataPin.mType))
                        hoveredPin = dataPin.mType;
                    ++pinId;
                }
                if (mDragPin && mDragPin->mDir == NodeGraph::PinDir::Out && mDragPin->mType == NodeGraph::PinType::Data) {
                    DataReceiverPin(nullptr, 0, pinId, *mDragType);
                }
                ed::EndNode();
                ed::PopStyleVar();

                uint32_t nodeId = 1;
                for (NodeGraph::NodeBase *node : uniquePtrToPtr(mGraph->nodes())) {
                    ed::BeginNode(6000 * nodeId);
                    ImGui::Text(node->name());

                    uint32_t inFlowCount = node->flowInCount();
                    uint32_t outFlowCount = node->flowOutCount();
                    for (uint32_t flowIndex = 0; flowIndex < max(inFlowCount, outFlowCount); ++flowIndex) {
                        if (flowIndex < inFlowCount)
                            FlowInPin(node->flowInName(flowIndex).data(), nodeId, flowIndex);
                        if (flowIndex < min(inFlowCount, outFlowCount))
                            ImGui::SameLine();
                        if (flowIndex < outFlowCount)
                            FlowOutPin(node->flowOutName(flowIndex).data(), nodeId, flowIndex);
                    }
                    uint32_t inDataCount = node->dataInCount();
                    uint32_t outDataCount = node->dataOutCount();
                    for (uint32_t dataIndex = 0; dataIndex < max(inDataCount, outDataCount); ++dataIndex) {
                        if (dataIndex < inDataCount) {
                            NodeGraph::TargetPin source = node->dataInSource(dataIndex);

                            ExtendedValueTypeDesc type = node->dataInType(dataIndex);

                            if (DataInPin(node->dataInName(dataIndex).data(), nodeId, dataIndex, type, source))
                                hoveredPin = type;
                        }
                        if (dataIndex < min(inDataCount, outDataCount))
                            ImGui::SameLine();
                        if (dataIndex < outDataCount) {
                            NodeGraph::TargetPin target = node->dataOutTarget(dataIndex);

                            ExtendedValueTypeDesc type = node->dataOutType(dataIndex);

                            if (DataOutPin(node->dataOutName(dataIndex).data(), nodeId, dataIndex, type, target))
                                hoveredPin = type;
                        }
                    }
                    uint32_t dataReceiverCount = node->dataReceiverCount();
                    uint32_t dataProviderCount = node->dataProviderCount();
                    for (uint32_t dataInstanceIndex = 0; dataInstanceIndex < max(dataReceiverCount, dataProviderCount); ++dataInstanceIndex) {
                        if (dataInstanceIndex < dataReceiverCount) {
                            ExtendedValueTypeDesc type = node->dataReceiverType(dataInstanceIndex);

                            if (DataReceiverPin(node->dataReceiverName(dataInstanceIndex).data(), nodeId, dataInstanceIndex, type))
                                hoveredPin = type;
                        }
                        if (dataInstanceIndex < min(dataReceiverCount, dataProviderCount))
                            ImGui::SameLine();
                        if (dataInstanceIndex < dataProviderCount) {
                            ExtendedValueTypeDesc type = node->dataProviderType(dataInstanceIndex);

                            if (DataProviderPin(node->dataProviderName(dataInstanceIndex).data(), nodeId, dataInstanceIndex, type))
                                hoveredPin = type;
                        }
                    }
                    ed::EndNode();

                    ++nodeId;
                }

                if (hoveredPin)
                    HoverPin(*hoveredPin);

                pinId = 0;
                for (NodeGraph::FlowOutPinPrototype &pin : mGraph->mFlowInPins) {
                    assert(pin.mTarget);
                    uint32_t id = NodeGraph::NodeBase::flowOutId(pinId);
                    ed::Link(id, id, 6000 * pin.mTarget.mNode + NodeGraph::NodeBase::flowInId(pin.mTarget.mIndex), ImColor { 255, 0, 0, 255 });
                    ++pinId;
                }
                pinId = 0;
                for (NodeGraph::DataInPinPrototype &pin : mGraph->mDataProviderPins) {
                    assert(pin.mSource);
                    uint32_t id = NodeGraph::NodeBase::dataInId(pinId);
                    ed::Link(id, 6000 * pin.mSource.mNode + NodeGraph::NodeBase::dataProviderId(pin.mSource.mIndex), id, ImColor { 0, 255, 0, 255 });
                    ++pinId;
                }
                pinId = 0;
                for (NodeGraph::DataOutPinPrototype &pin : mGraph->mDataReceiverPins) {
                    assert(pin.mTarget);
                    uint32_t id = NodeGraph::NodeBase::dataOutId(pinId);
                    ed::Link(id, id, 6000 * pin.mTarget.mNode + NodeGraph::NodeBase::dataReceiverId(pin.mTarget.mIndex), ImColor { 0, 255, 0, 255 });
                    ++pinId;
                }
                nodeId = 1;
                for (NodeGraph::NodeBase *node : uniquePtrToPtr(mGraph->nodes())) {
                    uint32_t outFlowCount = node->flowOutCount();
                    for (uint32_t flowIndex = 0; flowIndex < outFlowCount; ++flowIndex) {
                        if (NodeGraph::TargetPin target = node->flowOutTarget(flowIndex)) {
                            uint32_t id = 6000 * nodeId + NodeGraph::NodeBase::flowOutId(flowIndex);
                            ed::Link(id, id, 6000 * target.mNode + NodeGraph::NodeBase::flowInId(target.mIndex), ImColor { 255, 0, 0, 255 });
                        }
                    }
                    uint32_t inDataCount = node->dataInCount();
                    for (uint32_t dataIndex = 0; dataIndex < inDataCount; ++dataIndex) {
                        if (NodeGraph::TargetPin source = node->dataInSource(dataIndex)) {
                            uint32_t id = 6000 * nodeId + NodeGraph::NodeBase::dataInId(dataIndex);
                            ed::Link(id, 6000 * source.mNode + NodeGraph::NodeBase::dataProviderId(source.mIndex), id, ImColor { 0, 255, 0, 255 });
                        }
                    }
                    uint32_t outDataCount = node->dataOutCount();
                    for (uint32_t dataIndex = 0; dataIndex < outDataCount; ++dataIndex) {
                        if (NodeGraph::TargetPin target = node->dataOutTarget(dataIndex)) {
                            uint32_t id = 6000 * nodeId + NodeGraph::NodeBase::dataOutId(dataIndex);
                            ed::Link(id, id, 6000 * target.mNode + NodeGraph::NodeBase::dataReceiverId(target.mIndex), ImColor { 0, 255, 0, 255 });
                        }
                    }

                    ++nodeId;
                }

                mDragPin.reset();
                mDragType.reset();

                if (ed::BeginCreate()) {

                    ed::PinId inputPinId, outputPinId;
                    if (ed::QueryNewLink(&inputPinId, &outputPinId)) {

                        uintptr_t inputPinIdN = reinterpret_cast<uintptr_t>(inputPinId.AsPointer());
                        uint32_t inputNode = inputPinIdN / 6000;

                        NodeGraph::PinDesc inputPin = NodeGraph::NodeBase::pinFromId(inputPinIdN % 6000);

                        if (inputNode) {
                            mDragPin = inputPin;
                            if (inputPin.mType != NodeGraph::PinType::Flow) {
                                if (inputPin.mType == NodeGraph::PinType::Data) {
                                    if (inputPin.mDir == NodeGraph::PinDir::In) {
                                        mDragType = mGraph->node(inputNode)->dataInType(inputPin.mIndex);
                                    } else {
                                        mDragType = mGraph->node(inputNode)->dataOutType(inputPin.mIndex);
                                    }
                                } else {
                                    if (inputPin.mDir == NodeGraph::PinDir::In) {
                                        mDragType = mGraph->node(inputNode)->dataReceiverType(inputPin.mIndex);
                                    } else {
                                        mDragType = mGraph->node(inputNode)->dataProviderType(inputPin.mIndex);
                                    }
                                }
                            }
                        } else {
                            ed::RejectNewItem();
                        }

                        uintptr_t outputPinIdN = reinterpret_cast<uintptr_t>(outputPinId.AsPointer());
                        uint32_t outputNode = outputPinIdN / 6000;

                        NodeGraph::PinDesc outputPin = NodeGraph::NodeBase::pinFromId(outputPinIdN % 6000);

                        if ((outputPin.mType == NodeGraph::PinType::Flow && outputPin.mDir == NodeGraph::PinDir::In) || outputPin.mType == NodeGraph::PinType::DataInstance) {
                            std::swap(inputPin, outputPin);
                            std::swap(inputNode, outputNode);
                        }

                        //make this a < check
                        if (inputNode == outputNode) {
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        } else if (!outputPin.isCompatible(inputPin)) {
                            ShowLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        } else if (outputPin.mType == NodeGraph::PinType::Data && outputNode && inputNode && ((outputPin.mDir == NodeGraph::PinDir::Out && !mGraph->node(inputNode)->dataReceiverType(inputPin.mIndex).canAccept(mGraph->node(outputNode)->dataOutType(outputPin.mIndex))) || (outputPin.mDir == NodeGraph::PinDir::In && !mGraph->node(inputNode)->dataProviderType(inputPin.mIndex).canAccept(mGraph->node(outputNode)->dataInType(outputPin.mIndex))))) {
                            ShowLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        } else if ((outputPin.mType == NodeGraph::PinType::Flow && mGraph->flowOutTarget(outputNode, outputPin.mIndex)) || (outputPin.mType == NodeGraph::PinType::Data && ((outputPin.mDir == NodeGraph::PinDir::In && (mGraph->dataInSource(outputNode, outputPin.mIndex))) || (outputPin.mDir == NodeGraph::PinDir::Out && mGraph->dataOutTarget(outputNode, outputPin.mIndex))))) {
                            ShowLabel("x Cannot connect multiple links to pin", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        } else {
                            ShowLabel("+ Create Link", ImColor(32, 45, 32, 180));
                            if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f)) {
                                if (outputPin.mType == NodeGraph::PinType::Flow) {
                                    if (outputNode && inputNode)
                                        mGraph->node(outputNode)->connectFlow(outputPin.mIndex, inputNode, inputPin.mIndex);
                                    else
                                        mGraph->connectFlow(outputNode, outputPin.mIndex, inputNode, inputPin.mIndex);
                                } else {
                                    if (outputNode && inputNode) {
                                        if (outputPin.mDir == NodeGraph::PinDir::In)
                                            mGraph->node(outputNode)->connectDataIn(outputPin.mIndex, inputNode, inputPin.mIndex);
                                        else
                                            mGraph->node(outputNode)->connectDataOut(outputPin.mIndex, inputNode, inputPin.mIndex);
                                    } else {
                                        if (outputPin.mDir == NodeGraph::PinDir::In)
                                            mGraph->connectDataIn(outputNode, outputPin.mIndex, inputNode, inputPin.mIndex);
                                        else
                                            mGraph->connectDataOut(outputNode, outputPin.mIndex, inputNode, inputPin.mIndex);
                                    }
                                }
                            }
                        }
                    }
                    ed::PinId pinId = 0;
                    if (ed::QueryNewNode(&pinId)) {
                        uintptr_t pinIdN = reinterpret_cast<uintptr_t>(pinId.AsPointer());
                        size_t nodeId = pinIdN / 6000;
                        const NodeGraph::NodeBase *node = mGraph->nodes()[nodeId - 1].get();

                        if (nodeId == 0) {
                            ed::RejectNewItem();
                        } else {
                            mDragPin = NodeGraph::NodeBase::pinFromId(pinIdN % 6000);
                            if (mDragPin->mType != NodeGraph::PinType::Flow) {
                                if (mDragPin->mType == NodeGraph::PinType::Data) {
                                    if (mDragPin->mDir == NodeGraph::PinDir::In) {
                                        mDragType = node->dataInType(mDragPin->mIndex);
                                    } else {
                                        mDragType = node->dataOutType(mDragPin->mIndex);
                                    }
                                } else {
                                    if (mDragPin->mDir == NodeGraph::PinDir::In) {
                                        mDragType = node->dataReceiverType(mDragPin->mIndex);
                                    } else {
                                        mDragType = node->dataProviderType(mDragPin->mIndex);
                                    }
                                }
                            }
                        }
                    }
                }
                ed::EndCreate();

                if (ed::BeginDelete()) {
                    ed::LinkId linkId = 0;
                    while (ed::QueryDeletedLink(&linkId)) {
                        if (ed::AcceptDeletedItem()) {
                            uintptr_t pinIdN = reinterpret_cast<uintptr_t>(linkId.AsPointer());
                            NodeGraph::PinDesc pin = NodeGraph::NodeBase::pinFromId(pinIdN % 6000);
                            size_t nodeId = pinIdN / 6000;
                            if (nodeId == 0) {
                                if (pin.mType == NodeGraph::PinType::Flow) {
                                    mGraph->disconnectFlow(pin.mIndex);
                                } else {
                                    if (pin.mDir == NodeGraph::PinDir::In) {
                                        mGraph->disconnectDataIn(pin.mIndex);
                                    } else {
                                        mGraph->disconnectDataOut(pin.mIndex);
                                    }
                                }
                            } else {
                                NodeGraph::NodeBase *node = mGraph->nodes()[nodeId - 1].get();
                                if (pin.mType == NodeGraph::PinType::Flow) {
                                    node->disconnectFlow(pin.mIndex);
                                } else {
                                    if (pin.mDir == NodeGraph::PinDir::In) {
                                        node->disconnectDataIn(pin.mIndex);
                                    } else {
                                        node->disconnectDataOut(pin.mIndex);
                                    }
                                }
                            }
                        }
                    }

                    ed::NodeId nodeId = 0;
                    while (ed::QueryDeletedNode(&nodeId)) {
                        if (ed::AcceptDeletedItem()) {
                            NodeGraph::NodeBase *node = mGraph->nodes()[reinterpret_cast<uintptr_t>(nodeId.AsPointer<NodeGraph::NodeBase>()) / 6000 - 1].get();
                            mGraph->removeNode(node);
                        }
                    }
                }
                ed::EndDelete();

                ImVec2 popupPosition = ImGui::GetMousePos();
                ed::Suspend();
                /*if (ed::ShowNodeContextMenu(&contextNodeId))                
            else if (ed::ShowPinContextMenu(&contextPinId))
            else if (ed::ShowLinkContextMenu(&contextLinkId))                
            else */
                if (ed::ShowBackgroundContextMenu()) {
                    mPopupPosition = popupPosition;
                    ImGui::OpenPopup("Create New Node");
                }

                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
                if (ImGui::BeginPopup("Create New Node")) {

                    for (const std::pair<const std::string_view, IndexRef> &nodeDesc : NodeGraph::sNodesByName()) {
                        if (ImGui::MenuItem(nodeDesc.first.data())) {
                            NodeGraph::NodeBase *node = mGraph->addNode(NodeGraph::NodeRegistry::getConstructor(nodeDesc.second)(*mGraph));
                            ed::SetNodePosition(ed::NodeId { node }, mPopupPosition);
                        }
                    }

                    ImGui::EndPopup();
                }
                ImGui::PopStyleVar();

                ed::NodeId selectedNode[2];
                if (ed::GetSelectedNodes(selectedNode, 2) == 1) {
                    uintptr_t nodeId = reinterpret_cast<uintptr_t>(selectedNode[0].AsPointer<NodeGraph::NodeBase>());

                    if (nodeId < std::numeric_limits<int>::max() - 2) {
                        mSelectedNodeIndex = nodeId / 6000 - 1;
                    } else {
                        mSelectedNodeIndex.reset();
                    }
                } else {
                    mSelectedNodeIndex.reset();
                }

                ed::Resume();

                ed::End();

                ImGui::GetCurrentContext()->MouseViewport->Pos = oldViewportPos;
                ImGui::GetCurrentContext()->MouseViewport->Size = oldViewportSize;
            }
        }
        ImGui::End();

        if (mHierarchyVisible) {
            if (ImGui::Begin("Node graph - Hierarchy")) {
            }
            ImGui::End();
        }

        if (mNodeDetailsVisible) {
            if (ImGui::Begin("Node graph - Node Details")) {
                if (mSelectedNodeIndex) {
                    mIsDirty |= getTool<Inspector>().draw(mGraph->nodes()[mSelectedNodeIndex].get());
                }
            }
            ImGui::End();
        }

        if (!mGraph)
            ImGui::PopDisabled();
    }

    void NodeGraphEditor::renderMenu()
    {
        if (mVisible) {

            bool openNewGraphPopup = false;
            bool openGraphPopup = false;

            if (ImGui::BeginMenu("Node Graph Editor")) {

                if (ImGui::MenuItem("New Graph...")) {
                    openNewGraphPopup = true;
                }
                if (ImGui::MenuItem("Open Graph")) {
                    openGraphPopup = true;
                }
                if (ImGui::MenuItem("Save Graph", "", false, mGraph)) {
                    save();
                }

                if (ImGui::MenuItem("Debug", "", false, mGraph)) {
                    NodeGraph::NodeInterpreter interpreter { *mGraph };
                    interpreter.interpret();
                }

                ImGui::Separator();

                ImGui::MenuItem("Hierarchy", nullptr, &mHierarchyVisible);
                ImGui::MenuItem("Node Details", nullptr, &mNodeDetailsVisible);

                ImGui::EndMenu();
            }

            if (openNewGraphPopup) {
                mDirBuffer.clear();
                mSelectionBuffer.clear();
                ImGui::OpenPopup("NewGraph");
            }

            if (openGraphPopup) {
                ImGui::OpenPopup("OpenGraph");
            }

            ImGui::SetNextWindowSize({ 500, 400 }, ImGuiCond_FirstUseEver);
            if (ImGui::BeginPopupModal("NewGraph")) {

                bool accepted;
                if (ImGui::FilePicker(&mDirBuffer, &mSelectionBuffer, accepted, true)) {
                    if (accepted) {
                        Filesystem::remove(mSelectionBuffer);
                        create(mSelectionBuffer);
                    }
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            ImGui::SetNextWindowSize({ 500, 400 }, ImGuiCond_FirstUseEver);
            if (ImGui::BeginPopupModal("OpenGraph")) {

                bool alreadyClicked = false;

                ImGui::BeginChild("GraphList", { 0.0f, -ImGui::GetItemsLineHeightWithSpacing() });

                for (const std::pair<std::string_view, TypedScopePtr> &res : NodeGraph::NodeGraphLoader::getSingleton().resources()) {

                    bool selected = mSelectionTargetBuffer == res.first;

                    if (ImGui::Selectable(res.first.data(), selected)) {
                        mSelectionTargetBuffer = res.first;
                    }

                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                        mSelectionTargetBuffer = res.first;
                        alreadyClicked = true;
                    }
                }

                ImGui::EndChild();

                if (ImGui::Button("Cancel")) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (mSelectionTargetBuffer.empty())
                    ImGui::PushDisabled();
                if (ImGui::Button("Open") || alreadyClicked) {
                    load(mSelectionTargetBuffer);
                    ImGui::CloseCurrentPopup();
                }
                if (mSelectionTargetBuffer.empty())
                    ImGui::PopDisabled();

                ImGui::EndPopup();
            }
        }
    }

    std::string_view NodeGraphEditor::key() const
    {
        return "NodeGraphEditor";
    }

    void NodeGraphEditor::save()
    {
        mSaveQueued = true;
    }

    void NodeGraphEditor::load(const std::string &name)
    {
        mGraph.load(name);
        createEditor();
    }

    void NodeGraphEditor::create(const Filesystem::Path &path)
    {
        mGraph.create(path);
        createEditor();
    }

    bool NodeGraphEditor::onSave(const std::string_view &view, ed::SaveReasonFlags reason)
    {
        assert(mGraph);

        if (mInitialLoad) {
            mInitialLoad = false;
            return true;
        }

        if (!mSaveQueued) {
            mIsDirty = true;
            return false;
        }

        mSaveQueued = false;
        mIsDirty = false;

        OutStream layout = Filesystem::openFileWrite(layoutPath());
        layout.writeRaw(view.data(), view.size());

        mGraph->saveToFile();

        return true;
    }

    size_t NodeGraphEditor::onLoad(char *data)
    {
        assert(mGraph);

        Filesystem::Path path = layoutPath();

        size_t size = Filesystem::fileInfo(path).mSize;
        if (data) {
            InStream layout = Filesystem::openFileRead(path);
            layout.readRaw(data, size);
        }
        return size;
    }

    Filesystem::Path NodeGraphEditor::layoutPath() const
    {
        if (!mGraph)
            return {};

        return mGraph.resource()->path().parentPath() / (std::string { mGraph.resource()->name() } + ".json");
    }

    void NodeGraphEditor::createEditor()
    {
        if (mEditor) {
            ed::DestroyEditor(mEditor);
            mEditor = nullptr;
        }

        ed::Config config;

        config.UserPointer = this;

        config.SaveSettings = [](const char *data, size_t size, ed::SaveReasonFlags reason, void *userPointer) {
            return static_cast<NodeGraphEditor *>(userPointer)->onSave({ data, size }, reason);
        };

        config.LoadSettings = [](char *data, void *userPointer) {
            return static_cast<NodeGraphEditor *>(userPointer)->onLoad(data);
        };

        mEditor = ed::CreateEditor(&config);

        mInitialLoad = true;
    }

}
}

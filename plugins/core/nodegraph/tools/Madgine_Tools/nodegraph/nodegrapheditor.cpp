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

#include "Meta/keyvalue/valuetype.h"

#include "renderer/imroot.h"

UNIQUECOMPONENT(Engine::Tools::NodeGraphEditor);

METATABLE_BEGIN_BASE(Engine::Tools::NodeGraphEditor, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::NodeGraphEditor)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::NodeGraphEditor, Engine::Tools::ToolBase)
FIELD(mHierarchyVisible)
FIELD(mNodeDetailsVisible)
ENCAPSULATED_FIELD(Current, getCurrentName, load)
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

    ImColor DataColor(uint32_t mask)
    {
        switch (mask) {
        case NodeGraph::NodeExecutionMask::NONE:
            return { 50, 50, 50, 255 };
            break;
        case NodeGraph::NodeExecutionMask::CPU:
            return { 0, 255, 0, 255 };
            break;
        case NodeGraph::NodeExecutionMask::GPU:
            return { 160, 160, 255, 255 };
            break;
        case NodeGraph::NodeExecutionMask::ALL:
            return { 0, 255, 255, 255 };
            break;
        default:
            throw 0;
        }
    }

    ImColor FlowColor(uint32_t mask)
    {
        switch (mask) {
        case NodeGraph::NodeExecutionMask::NONE:
            return { 50, 50, 50, 255 };
            break;
        case NodeGraph::NodeExecutionMask::CPU:
            return { 255, 0, 0, 255 };
            break;
        case NodeGraph::NodeExecutionMask::GPU:
            return { 200, 200, 0, 255 };
            break;
        case NodeGraph::NodeExecutionMask::ALL:
            return { 255, 127, 0, 255 };
            break;
        default:
            throw 0;
        }
    }

    void DataPinIcon(ExtendedValueTypeDesc type, uint32_t mask, bool connected = false)
    {
        ax::Widgets::Icon(ImVec2(sPinIconSize, sPinIconSize), ax::Widgets::IconType::Circle, connected, DataColor(mask), ImColor(32, 32, 32, 255));
        ImVec2 align { 0.5f, 0.5f };
        ImVec2 pos = ImGui::GetItemRectMin() + align * ImGui::GetItemRectSize();
        ed::PinPivotRect(pos, pos);
    }

    void DataInstancePinIcon(ExtendedValueTypeDesc type, uint32_t mask, bool connected = false)
    {
        ax::Widgets::Icon(ImVec2(sPinIconSize, sPinIconSize), ax::Widgets::IconType::Square, connected, DataColor(mask), ImColor(32, 32, 32, 255));
        ImVec2 align { 0.5f, 0.5f };
        ImVec2 pos = ImGui::GetItemRectMin() + align * ImGui::GetItemRectSize();
        ed::PinPivotRect(pos, pos);
    }

    void FlowPinIcon(uint32_t mask)
    {
        ax::Widgets::Icon(ImVec2(sPinIconSize, sPinIconSize), ax::Widgets::IconType::Flow, false, FlowColor(mask), ImColor(32, 32, 32, 255));
        ImVec2 align { 0.5f, 0.5f };
        ImVec2 pos = ImGui::GetItemRectMin() + align * ImGui::GetItemRectSize();
        ed::PinPivotRect(pos, pos);
    }

    void FlowOutPin(const char *name, uint32_t nodeId, uint32_t pinId, uint32_t mask)
    {
        ed::BeginPin(6000 * nodeId + NodeGraph::NodeBase::flowOutId(pinId), ed::PinKind::Output);
        if (name) {
            ImGui::Text("%s", name);
            ImGui::SameLine();
        }
        FlowPinIcon(mask);
        ed::EndPin();
    }

    void FlowInPin(const char *name, uint32_t nodeId, uint32_t pinId, uint32_t mask)
    {
        ed::BeginPin(6000 * nodeId + NodeGraph::NodeBase::flowInId(pinId), ed::PinKind::Input);
        FlowPinIcon(mask);
        if (name) {
            ImGui::SameLine();
            ImGui::Text("%s", name);
        }
        ed::EndPin();
    }

    bool DataOutPin(const char *name, size_t nodeId, size_t pinId, ExtendedValueTypeDesc type, uint32_t mask, const NodeGraph::Pin &target)
    {
        ed::BeginPin(6000 * nodeId + NodeGraph::NodeBase::dataOutId(pinId), ed::PinKind::Output);
        if (name) {
            ImGui::Text("%s", name);
            ImGui::SameLine();
        }
        DataPinIcon(type, mask, bool(target));
        ed::EndPin();
        return ImGui::IsItemHovered();
    }

    bool DataInPin(const char *name, size_t nodeId, size_t pinId, ExtendedValueTypeDesc type, uint32_t mask, const NodeGraph::Pin &source)
    {
        uintptr_t id = 6000 * nodeId + NodeGraph::NodeBase::dataInId(pinId);
        ed::BeginPin(id, ed::PinKind::Input);
        DataPinIcon(type, mask, bool(source));
        if (name) {
            ImGui::SameLine();
            ImGui::Text("%s", name);
        }
        ed::EndPin();
        return ImGui::IsItemHovered();
    }

    bool DataProviderPin(const char *name, size_t nodeId, size_t pinId, ExtendedValueTypeDesc type, uint32_t mask)
    {
        ed::BeginPin(6000 * nodeId + NodeGraph::NodeBase::dataProviderId(pinId), ed::PinKind::Output);
        if (name) {
            ImGui::Text("%s", name);
            ImGui::SameLine();
        }
        DataInstancePinIcon(type, mask);
        ed::EndPin();
        return ImGui::IsItemHovered();
    }

    bool DataReceiverPin(const char *name, size_t nodeId, size_t pinId, ExtendedValueTypeDesc type, uint32_t mask)
    {
        uintptr_t id = 6000 * nodeId + NodeGraph::NodeBase::dataReceiverId(pinId);
        ed::BeginPin(id, ed::PinKind::Input);
        DataInstancePinIcon(type, mask);
        if (name) {
            ImGui::SameLine();
            ImGui::Text("%s", name);
        }
        ed::EndPin();
        return ImGui::IsItemHovered();
    }

    void HoverPin(ExtendedValueTypeDesc type)
    {
        ImVec2 cursor = ImGui::GetCursorPos();
        ImGui::SetCursorScreenPos(ImGui::GetMousePos());
        if (type.mType.isRegular()) {
            ShowLabel(type.toString());
        } else {
            ShowLabel("*");
        }
        ImGui::SetCursorPos(cursor);
    }

    NodeGraphEditor::NodeGraphEditor(ImRoot &root)
        : Tool<NodeGraphEditor>(root)
    {
    }

    Threading::Task<bool> NodeGraphEditor::init()
    {
        createEditor();

        co_return co_await ToolBase::init();
    }

    Threading::Task<void> NodeGraphEditor::finalize()
    {
        mGraphHandle.reset();
        mEditor.reset();

        co_await ToolBase::finalize();
    }

    void NodeGraphEditor::render()
    {
        ImGui::PushID(this);

        std::string fileName;
        if (mGraphHandle.available()) {
            fileName = mGraphHandle.resource()->path().filename().str();
        } else {
            fileName = "Node graph";
        }
        fileName += "###editor";

        ImGuiWindowFlags flags = 0;
        if (mIsDirty)
            flags |= ImGuiWindowFlags_UnsavedDocument;

        if (ImGui::Begin(fileName.c_str(), nullptr, flags)) {

            ImVec2 topLeftScreen = ImGui::GetCursorScreenPos();

            //Probably set this in the implementation to the exact Canvas-View area
            ImVec2 oldViewportPos = ImGui::GetCurrentContext()->MouseViewport->Pos;
            ImVec2 oldViewportSize = ImGui::GetCurrentContext()->MouseViewport->Size;

            ImGui::GetCurrentContext()->MouseViewport->Pos = { -10000, -10000 };
            ImGui::GetCurrentContext()->MouseViewport->Size = { 20000, 20000 };

            ed::SetCurrentEditor(mEditor.get());

            std::optional<ExtendedValueTypeDesc> hoveredPin;

            ed::Begin("Node editor");

            auto alpha = ImGui::GetStyle().Alpha;

            ed::PushStyleVar(ed::StyleVar_NodePadding, { 0, 0, 0, 0 });
            ImVec2 specialPosition = ed::ScreenToCanvas(topLeftScreen);
            ed::SetNodePosition(std::numeric_limits<int>::max() - 1, { floorf(specialPosition.x), floorf(specialPosition.y) });
            ed::BeginNode(std::numeric_limits<int>::max() - 1);
            ImGui::Dummy({ sPinIconSize, 10 });
            uint32_t pinId = 0;
            for (NodeGraph::FlowOutPinPrototype &flowPin : mGraph.mFlowOutPins) {
                FlowOutPin(nullptr, 0, pinId, mGraph.node(flowPin.mTarget.mNode)->flowInMask(flowPin.mTarget.mIndex));
                ++pinId;
            }
            if (mDragPin && mDragPin->mDir == NodeGraph::PinDir::In && mDragPin->mType == NodeGraph::PinType::Flow) {
                FlowOutPin(nullptr, 0, pinId, mDragMask);
            }
            pinId = 0;
            for (NodeGraph::DataOutPinPrototype &dataPin : mGraph.mDataOutPins) {
                NodeGraph::NodeBase *node = mGraph.node(dataPin.mTarget.mNode);
                ExtendedValueTypeDesc type = node->dataOutType(dataPin.mTarget.mIndex);
                if (DataOutPin(nullptr, 0, pinId, type, node->dataReceiverMask(dataPin.mTarget.mIndex), dataPin.mTarget))
                    hoveredPin = type;
                ++pinId;
            }
            if (mDragPin && mDragPin->mDir == NodeGraph::PinDir::In && mDragPin->mType == NodeGraph::PinType::DataInstance) {
                DataOutPin(nullptr, 0, pinId, *mDragType, mDragMask, {});
            }
            pinId = 0;
            for (NodeGraph::DataProviderPinPrototype &dataPin : mGraph.mDataProviderPins) {
                NodeGraph::NodeBase *node = mGraph.node(dataPin.mTargets.front().mNode);
                ExtendedValueTypeDesc type = mGraph.dataProviderType({ 0, pinId });
                if (DataProviderPin(nullptr, 0, pinId, type, node->dataInMask(dataPin.mTargets.front().mIndex)))
                    hoveredPin = type;
                ++pinId;
            }
            if (mDragPin && mDragPin->mDir == NodeGraph::PinDir::In && mDragPin->mType == NodeGraph::PinType::Data) {
                DataProviderPin(nullptr, 0, pinId, *mDragType, mDragMask);
            }
            ed::EndNode();

            specialPosition = ed::ScreenToCanvas({ topLeftScreen.x + ed::GetScreenSize().x - sPinIconSize / ed::GetCurrentZoom(), topLeftScreen.y });
            ed::SetNodePosition(std::numeric_limits<int>::max() - 2, { floorf(specialPosition.x), floorf(specialPosition.y) });
            ed::BeginNode(std::numeric_limits<int>::max() - 2);
            ImGui::Dummy({ sPinIconSize, 10 });
            pinId = 0;
            for (NodeGraph::FlowInPinPrototype &flowPin : mGraph.mFlowInPins) {
                NodeGraph::NodeBase *node = mGraph.node(flowPin.mSources.front().mNode);
                FlowInPin(nullptr, 0, pinId, node->flowInMask(flowPin.mSources.front().mIndex));
                ++pinId;
            }
            if (mDragPin && mDragPin->mDir == NodeGraph::PinDir::Out && mDragPin->mType == NodeGraph::PinType::Flow) {
                FlowInPin(nullptr, 0, pinId, mDragMask);
            }
            pinId = 0;
            for (NodeGraph::DataInPinPrototype &dataPin : mGraph.mDataInPins) {
                assert(dataPin.mSource && dataPin.mSource.mNode);
                NodeGraph::NodeBase *node = mGraph.node(dataPin.mSource.mNode);
                ExtendedValueTypeDesc type = node->dataProviderType(dataPin.mSource.mIndex);

                if (DataInPin(nullptr, 0, pinId, type, node->dataProviderMask(dataPin.mSource.mIndex), dataPin.mSource))
                    hoveredPin = type;
                ++pinId;
            }
            if (mDragPin && mDragPin->mDir == NodeGraph::PinDir::Out && mDragPin->mType == NodeGraph::PinType::DataInstance) {
                DataInPin(nullptr, 0, pinId, *mDragType, mDragMask, {});
            }
            pinId = 0;
            for (NodeGraph::DataReceiverPinPrototype &dataPin : mGraph.mDataReceiverPins) {
                NodeGraph::NodeBase *node = mGraph.node(dataPin.mSources.front().mNode);
                ExtendedValueTypeDesc type = mGraph.dataOutType({ 0, pinId });
                if (DataReceiverPin(nullptr, 0, pinId, type, node->dataOutMask(dataPin.mSources.front().mNode)))
                    hoveredPin = type;
                ++pinId;
            }
            if (mDragPin && mDragPin->mDir == NodeGraph::PinDir::Out && mDragPin->mType == NodeGraph::PinType::Data) {
                DataReceiverPin(nullptr, 0, pinId, *mDragType, mDragMask);
            }
            ed::EndNode();
            ed::PopStyleVar();

            uint32_t nodeId = 1;
            NodeGraph::NodeBase *hoveredNode = nullptr;
            for (NodeGraph::NodeBase *node : uniquePtrToPtr(mGraph.nodes())) {
                ed::BeginNode(6000 * nodeId);
                ImGui::Text(node->name());

                auto it = mNodeMessages.find(node);
                if (it != mNodeMessages.end()) {
                    bool first = true;
                    if (!it->second.mErrorMessages.empty()) {
                        if (first)
                            first = false;
                        else
                            ImGui::SameLine();
                        ImGui::TextColored({ 1, 0, 0, 1 }, "%s", std::to_string(it->second.mErrorMessages.size()).c_str());
                    }
                    if (!it->second.mWarningMessages.empty()) {
                        if (first)
                            first = false;
                        else
                            ImGui::SameLine();
                        ImGui::TextColored({ 1, 1, 0, 1 }, "%s", std::to_string(it->second.mWarningMessages.size()).c_str());
                    }
                }

                std::string id = std::to_string((uintptr_t)node);
                ImGui::BeginColumns(id.c_str(), 2, ImGuiColumnsFlags_NoBorder | ImGuiColumnsFlags_GrowParentContentsSize);

                ImGui::SetColumnWidth(0, 125.0f);
                ImGui::SetColumnWidth(1, 125.0f);

                uint32_t inFlowCount = node->flowInCount();
                uint32_t outFlowCount = node->flowOutCount();
                for (uint32_t flowIndex = 0; flowIndex < max(inFlowCount, outFlowCount); ++flowIndex) {
                    if (flowIndex < inFlowCount)
                        FlowInPin(node->flowInName(flowIndex).data(), nodeId, flowIndex, node->flowInMask(flowIndex));

                    ImGui::NextColumn();
                    if (flowIndex < outFlowCount)
                        FlowOutPin(node->flowOutName(flowIndex).data(), nodeId, flowIndex, node->flowOutMask(flowIndex));

                    ImGui::NextColumn();
                }
                uint32_t inDataCount = node->dataInCount();
                uint32_t outDataCount = node->dataOutCount();
                for (uint32_t dataIndex = 0; dataIndex < max(inDataCount, outDataCount); ++dataIndex) {
                    if (dataIndex < inDataCount) {
                        NodeGraph::Pin source = node->dataInSource(dataIndex);

                        ExtendedValueTypeDesc type = node->dataInType(dataIndex);

                        if (DataInPin(node->dataInName(dataIndex).data(), nodeId, dataIndex, type, node->dataInMask(dataIndex), source))
                            hoveredPin = type;
                    }

                    ImGui::NextColumn();

                    if (dataIndex < outDataCount) {
                        NodeGraph::Pin target = node->dataOutTarget(dataIndex);

                        ExtendedValueTypeDesc type = node->dataOutType(dataIndex);

                        if (DataOutPin(node->dataOutName(dataIndex).data(), nodeId, dataIndex, type, node->dataOutMask(dataIndex), target))
                            hoveredPin = type;
                    }

                    ImGui::NextColumn();
                }
                if (mDragPin && mDragPin->mType == NodeGraph::PinType::DataInstance && ((node->dataInVariadic() && mDragPin->mDir == NodeGraph::PinDir::Out) || (node->dataOutVariadic() && mDragPin->mDir == NodeGraph::PinDir::In))) {
                    if (node->dataInVariadic() && mDragPin->mDir == NodeGraph::PinDir::Out) {
                        uint32_t index = node->dataInCount();

                        if (DataInPin(node->dataInName(index).data(), nodeId, index, *mDragType, node->dataInMask(index), {}))
                            hoveredPin = *mDragType;
                    }

                    ImGui::NextColumn();

                    if (node->dataOutVariadic() && mDragPin->mDir == NodeGraph::PinDir::In) {
                    }

                    ImGui::NextColumn();
                }

                uint32_t dataReceiverCount = node->dataReceiverCount();
                uint32_t dataProviderCount = node->dataProviderCount();
                for (uint32_t dataInstanceIndex = 0; dataInstanceIndex < max(dataReceiverCount, dataProviderCount); ++dataInstanceIndex) {
                    if (dataInstanceIndex < dataReceiverCount) {
                        ExtendedValueTypeDesc type = node->dataReceiverType(dataInstanceIndex);

                        if (DataReceiverPin(node->dataReceiverName(dataInstanceIndex).data(), nodeId, dataInstanceIndex, type, node->dataReceiverMask(dataInstanceIndex)))
                            hoveredPin = type;
                    }

                    ImGui::NextColumn();

                    if (dataInstanceIndex < dataProviderCount) {
                        ExtendedValueTypeDesc type = node->dataProviderType(dataInstanceIndex);

                        if (DataProviderPin(node->dataProviderName(dataInstanceIndex).data(), nodeId, dataInstanceIndex, type, node->dataProviderMask(dataInstanceIndex)))
                            hoveredPin = type;
                    }

                    ImGui::NextColumn();
                }
                if (mDragPin && mDragPin->mType == NodeGraph::PinType::Data && ((node->dataReceiverVariadic() && mDragPin->mDir == NodeGraph::PinDir::Out) || (node->dataProviderVariadic() && mDragPin->mDir == NodeGraph::PinDir::In))) {
                    if (node->dataReceiverVariadic() && mDragPin->mDir == NodeGraph::PinDir::Out) {
                        uint32_t index = node->dataReceiverCount();

                        if (DataReceiverPin(node->dataReceiverName(index).data(), nodeId, index, *mDragType, node->dataReceiverMask(index)))
                            hoveredPin = *mDragType;
                    }

                    ImGui::NextColumn();

                    if (node->dataProviderVariadic() && mDragPin->mDir == NodeGraph::PinDir::In) {
                    }

                    ImGui::NextColumn();
                }

                ImGui::EndColumns();
                ed::EndNode();

                if (ImGui::IsItemHovered()) {
                    if (it != mNodeMessages.end()) {
                        hoveredNode = node;
                    }
                }

                ++nodeId;
            }

            if (hoveredPin)
                HoverPin(*hoveredPin);

            pinId = 0;
            for (NodeGraph::FlowOutPinPrototype &pin : mGraph.mFlowOutPins) {
                assert(pin.mTarget);
                uint32_t id = NodeGraph::NodeBase::flowOutId(pinId);
                ed::Link(id, id, 6000 * pin.mTarget.mNode + NodeGraph::NodeBase::flowInId(pin.mTarget.mIndex), ImColor { 255, 0, 0, 255 });
                ++pinId;
            }
            pinId = 0;
            for (NodeGraph::DataInPinPrototype &pin : mGraph.mDataInPins) {
                assert(pin.mSource);
                uint32_t id = NodeGraph::NodeBase::dataInId(pinId);
                ed::Link(id, 6000 * pin.mSource.mNode + NodeGraph::NodeBase::dataProviderId(pin.mSource.mIndex), id, ImColor { 0, 255, 0, 255 });
                ++pinId;
            }
            pinId = 0;
            for (NodeGraph::DataOutPinPrototype &pin : mGraph.mDataOutPins) {
                assert(pin.mTarget);
                uint32_t id = NodeGraph::NodeBase::dataOutId(pinId);
                ed::Link(id, id, 6000 * pin.mTarget.mNode + NodeGraph::NodeBase::dataReceiverId(pin.mTarget.mIndex), ImColor { 0, 255, 0, 255 });
                ++pinId;
            }
            nodeId = 1;
            for (NodeGraph::NodeBase *node : uniquePtrToPtr(mGraph.nodes())) {
                uint32_t outFlowCount = node->flowOutCount();
                for (uint32_t flowIndex = 0; flowIndex < outFlowCount; ++flowIndex) {
                    if (NodeGraph::Pin target = node->flowOutTarget(flowIndex)) {
                        uint32_t id = 6000 * nodeId + NodeGraph::NodeBase::flowOutId(flowIndex);
                        ed::Link(id, id, 6000 * target.mNode + NodeGraph::NodeBase::flowInId(target.mIndex), FlowColor(node->flowOutMask(flowIndex)));
                    }
                }
                uint32_t inDataCount = node->dataInCount();
                for (uint32_t dataIndex = 0; dataIndex < inDataCount; ++dataIndex) {
                    if (NodeGraph::Pin source = node->dataInSource(dataIndex)) {
                        uint32_t id = 6000 * nodeId + NodeGraph::NodeBase::dataInId(dataIndex);
                        ed::Link(id, 6000 * source.mNode + NodeGraph::NodeBase::dataProviderId(source.mIndex), id, DataColor(node->dataInMask(dataIndex)));
                    }
                }
                uint32_t outDataCount = node->dataOutCount();
                for (uint32_t dataIndex = 0; dataIndex < outDataCount; ++dataIndex) {
                    if (NodeGraph::Pin target = node->dataOutTarget(dataIndex)) {
                        uint32_t id = 6000 * nodeId + NodeGraph::NodeBase::dataOutId(dataIndex);
                        ed::Link(id, id, 6000 * target.mNode + NodeGraph::NodeBase::dataReceiverId(target.mIndex), DataColor(node->dataOutMask(dataIndex)));
                    }
                }

                ++nodeId;
            }

            if (ed::BeginCreate()) {

                queryLink();

                ed::PinId pinId = 0;
                if (ed::QueryNewNode(&pinId)) {
                    uintptr_t pinIdN = pinId.Get();

                    NodeGraph::PinDesc pin = NodeGraph::NodeBase::pinFromId(pinIdN);

                    if (!pin.mPin.mNode) {
                        ed::RejectNewItem();
                    } else {

                        mDragPin = pin;
                        switch (pin.mType) {
                        case NodeGraph::PinType::Data:
                            if (pin.mDir == NodeGraph::PinDir::In) {
                                mDragType = mGraph.dataInType(pin.mPin);
                                mDragMask = mGraph.dataInMask(pin.mPin);
                            } else {
                                mDragType = mGraph.dataOutType(pin.mPin);
                                mDragMask = mGraph.dataOutMask(pin.mPin);
                            }
                            break;
                        case NodeGraph::PinType::DataInstance:
                            if (pin.mDir == NodeGraph::PinDir::In) {
                                mDragType = mGraph.dataReceiverType(pin.mPin);
                                mDragMask = mGraph.dataReceiverMask(pin.mPin);
                            } else {
                                mDragType = mGraph.dataProviderType(pin.mPin);
                                mDragMask = mGraph.dataProviderMask(pin.mPin);
                            }
                            break;
                        case NodeGraph::PinType::Flow:
                            if (pin.mDir == NodeGraph::PinDir::In) {
                                mDragMask = mGraph.flowInMask(pin.mPin);
                            } else {
                                mDragMask = mGraph.flowOutMask(pin.mPin);
                            }
                            break;
                        default:
                            throw 0;
                        }
                    }
                }
            } else {
                mDragPin.reset();
                mDragType.reset();
            }
            ed::EndCreate();

            if (ed::BeginDelete()) {
                ed::LinkId linkId = 0;
                while (ed::QueryDeletedLink(&linkId)) {
                    if (ed::AcceptDeletedItem()) {
                        uintptr_t pinIdN = linkId.Get();
                        NodeGraph::PinDesc pin = NodeGraph::NodeBase::pinFromId(pinIdN);

                        if (pin.mType == NodeGraph::PinType::Flow) {
                            mGraph.disconnectFlow(pin.mPin);
                        } else {
                            if (pin.mDir == NodeGraph::PinDir::In) {
                                mGraph.disconnectDataIn(pin.mPin);
                            } else {
                                mGraph.disconnectDataOut(pin.mPin);
                            }
                        }
                    }
                }

                ed::NodeId nodeId = 0;
                while (ed::QueryDeletedNode(&nodeId)) {
                    if (ed::AcceptDeletedItem()) {
                        mGraph.removeNode(nodeId.Get() / 6000);
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

                for (const std::pair<const std::string_view, size_t> &nodeDesc : NodeGraph::NodeRegistry::sComponentsByName()) {
                    if (ImGui::MenuItem(nodeDesc.first.data())) {
                        NodeGraph::NodeBase *node = mGraph.addNode(NodeGraph::NodeRegistry::getConstructor(nodeDesc.second)(mGraph));
                        ed::SetNodePosition(ed::NodeId { node }, mPopupPosition);
                    }
                }

                ImGui::EndPopup();
            }
            ImGui::PopStyleVar();

            ed::NodeId selectedNode[2];
            if (ed::GetSelectedNodes(selectedNode, 2) == 1) {
                uintptr_t nodeId = selectedNode[0].Get();

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

            if (hoveredNode) {
                auto it = mNodeMessages.find(hoveredNode);
                if (it != mNodeMessages.end()) {
                    ImGui::BeginTooltip();
                    for (const std::string &msg : it->second.mErrorMessages) {
                        ImGui::TextColored({ 1, 0, 0, 1 }, "%s", msg.c_str());
                    }
                    for (const std::string &msg : it->second.mWarningMessages) {
                        ImGui::TextColored({ 1, 1, 0, 1 }, "%s", msg.c_str());
                    }
                    ImGui::EndTooltip();
                }
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
                    mIsDirty |= getTool<Inspector>().drawMembers(mGraph.nodes()[mSelectedNodeIndex].get());
                }
            }
            ImGui::End();
        }

        ImGui::PopID();
    }

    void NodeGraphEditor::renderMenu()
    {
        ToolBase::renderMenu();
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
                if (ImGui::MenuItem("Save Graph", "", false)) {
                    save();
                }

                if (ImGui::MenuItem("Debug", "", false)) {
                    NodeGraph::NodeInterpreter interpreter { &mGraph };
                    IndexType<uint32_t> index = 0;
                    interpreter.interpret(index, {});
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

                ImGui::BeginChild("GraphList", { 0.0f, -ImGui::GetFrameHeightWithSpacing() });

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

    void NodeGraphEditor::load(std::string_view name)
    {
        mGraphHandle.load(name).then([this](bool b) {
            if (b)
                mGraph = *mGraphHandle;
            else
                mGraph = {};
            createEditor();
        },
            mRoot.taskQueue());
    }

    void NodeGraphEditor::create(const Filesystem::Path &path)
    {
        mGraphHandle.create(path).then([this](bool b) {
            if (b)
                mGraph = *mGraphHandle;
            else
                mGraph = {};
            createEditor();
        },
            mRoot.taskQueue());
    }

    std::string_view NodeGraphEditor::getCurrentName() const
    {
        return mGraphHandle.available() ? mGraphHandle->name() : "";
    }

    bool NodeGraphEditor::onSave(std::string_view view, ed::SaveReasonFlags reason)
    {
        verify();

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

        if (!mGraphHandle)
            throw 0;

        Stream layout = Filesystem::openFileWrite(layoutPath());
        layout.write(view.data(), view.size());

        mGraph.saveToFile();

        return true;
    }

    size_t NodeGraphEditor::onLoad(char *data)
    {
        if (!mGraphHandle)
            return 0;

        Filesystem::Path path = layoutPath();

        size_t size = Filesystem::fileInfo(path).mSize;
        if (data) {
            Stream layout = Filesystem::openFileRead(path);
            layout.read(data, size);
        }
        return size;
    }

    Filesystem::Path NodeGraphEditor::layoutPath() const
    {
        if (!mGraphHandle)
            return {};

        return mGraphHandle.resource()->path().parentPath() / (std::string { mGraphHandle.resource()->name() } + ".json");
    }

    void NodeGraphEditor::createEditor()
    {
        mEditor.reset();

        ed::Config config;

        config.UserPointer = this;

        config.SaveSettings = [](const char *data, size_t size, ed::SaveReasonFlags reason, void *userPointer) {
            return static_cast<NodeGraphEditor *>(userPointer)->onSave({ data, size }, reason);
        };

        config.LoadSettings = [](char *data, void *userPointer) {
            return static_cast<NodeGraphEditor *>(userPointer)->onLoad(data);
        };

        mEditor = { ed::CreateEditor(&config), &ed::DestroyEditor };

        mInitialLoad = true;
    }

    void NodeGraphEditor::queryLink()
    {
        ed::PinId inputPinId, outputPinId;
        if (ed::QueryNewLink(&inputPinId, &outputPinId)) {

            uintptr_t inputPinIdN = inputPinId.Get();

            NodeGraph::PinDesc inputPin = NodeGraph::NodeBase::pinFromId(inputPinIdN);

            mDragPin = inputPin;
            switch (inputPin.mType) {
            case NodeGraph::PinType::Data:
                if (inputPin.mDir == NodeGraph::PinDir::In) {
                    mDragType = mGraph.dataInType(inputPin.mPin);
                    mDragMask = mGraph.dataInMask(inputPin.mPin);
                } else {
                    mDragType = mGraph.dataOutType(inputPin.mPin);
                    mDragMask = mGraph.dataOutMask(inputPin.mPin);
                }
                break;
            case NodeGraph::PinType::DataInstance:
                if (inputPin.mDir == NodeGraph::PinDir::In) {
                    mDragType = mGraph.dataReceiverType(inputPin.mPin);
                    mDragMask = mGraph.dataReceiverMask(inputPin.mPin);
                } else {
                    mDragType = mGraph.dataProviderType(inputPin.mPin);
                    mDragMask = mGraph.dataProviderMask(inputPin.mPin);
                }
                break;
            case NodeGraph::PinType::Flow:
                if (inputPin.mDir == NodeGraph::PinDir::In) {
                    mDragMask = mGraph.flowInMask(inputPin.mPin);
                } else {
                    mDragMask = mGraph.flowOutMask(inputPin.mPin);
                }
                break;
            default:
                throw 0;
            }

            uintptr_t outputPinIdN = outputPinId.Get();

            NodeGraph::PinDesc outputPin = NodeGraph::NodeBase::pinFromId(outputPinIdN);

            if ((outputPin.mType == NodeGraph::PinType::Flow && outputPin.mDir == NodeGraph::PinDir::In) || outputPin.mType == NodeGraph::PinType::DataInstance) {
                std::swap(inputPin, outputPin);
            }

            //make this a < check
            if (inputPin.mPin.mNode == outputPin.mPin.mNode) {
                ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                return;
            }

            if (!outputPin.isCompatible(inputPin)) {
                ShowLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                return;
            }

            uint32_t inputMask, outputMask;
            if (outputPin.mType == NodeGraph::PinType::Data) {
                ExtendedValueTypeDesc inputType = outputPin.mDir == NodeGraph::PinDir::Out ? mGraph.dataReceiverType(inputPin.mPin) : mGraph.dataProviderType(inputPin.mPin);
                ExtendedValueTypeDesc outputType = outputPin.mDir == NodeGraph::PinDir::Out ? mGraph.dataOutType(outputPin.mPin) : mGraph.dataInType(outputPin.mPin);

                if (!inputType.isCompatible(outputType)) {
                    ShowLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                    ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                    return;
                }

                if ((outputPin.mDir == NodeGraph::PinDir::In && (mGraph.dataInSource(outputPin.mPin))) || (outputPin.mDir == NodeGraph::PinDir::Out && mGraph.dataOutTarget(outputPin.mPin))) {
                    ShowLabel("x Cannot connect multiple links to pin", ImColor(45, 32, 32, 180));
                    ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                    return;
                }

                inputMask = outputPin.mDir == NodeGraph::PinDir::Out ? mGraph.dataReceiverMask(inputPin.mPin) : mGraph.dataProviderMask(inputPin.mPin);
                outputMask = outputPin.mDir == NodeGraph::PinDir::Out ? mGraph.dataOutMask(outputPin.mPin) : mGraph.dataInMask(outputPin.mPin);
            } else {
                if (mGraph.flowOutTarget(outputPin.mPin)) {
                    ShowLabel("x Cannot connect multiple links to pin", ImColor(45, 32, 32, 180));
                    ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                    return;
                }

                inputMask = mGraph.flowInMask(inputPin.mPin);
                outputMask = mGraph.flowOutMask(outputPin.mPin);
            }

            if (!(inputMask & outputMask)) {
                ShowLabel("x Incompatible Execution Masks", ImColor(45, 32, 32, 180));
                ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                return;
            }

            ShowLabel("+ Create Link", ImColor(32, 45, 32, 180));
            if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f)) {
                if (outputPin.mType == NodeGraph::PinType::Flow) {
                    mGraph.connectFlow(outputPin.mPin, inputPin.mPin);
                } else {
                    if (outputPin.mDir == NodeGraph::PinDir::In)
                        mGraph.connectDataIn(outputPin.mPin, inputPin.mPin);
                    else
                        mGraph.connectDataOut(outputPin.mPin, inputPin.mPin);
                }
            }
        }
    }

    void NodeGraphEditor::verify()
    {
        for (const std::unique_ptr<NodeGraph::NodeBase> &node : mGraph.nodes()) {
            NodeMessages messages;

            for (uint32_t i = 0; i < node->dataInCount(); ++i) {
                if (!node->dataInSource(i) && node->dataInDefault(i).is<std::monostate>()) {
                    std::stringstream ss;
                    ss << "No Input provided for pin '" << node->dataInName(i) << "'!";
                    messages.mErrorMessages.push_back(ss.str());
                }
            }

            if (messages.mErrorMessages.empty() && messages.mWarningMessages.empty()) {
                mNodeMessages.erase(node.get());
            } else {
                auto pib = mNodeMessages.try_emplace(node.get(), std::move(messages));
                if (!pib.second)
                    pib.first->second = std::move(messages);
            }
        }
    }

}
}

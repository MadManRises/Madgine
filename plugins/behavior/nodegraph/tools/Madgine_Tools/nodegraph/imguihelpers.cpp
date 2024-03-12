#include "../nodegraphtoolslib.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imguiaddons.h"

#include "imguihelpers.h"

#include "Madgine/nodegraph/nodeexecution.h"

#include "NodeEditor/ax/Widgets.h"
#include "NodeEditor/imgui_node_editor.h"

namespace Engine {
namespace Tools {

    

	void ShowLabel(std::string_view label, ImColor color)
    {
        ImVec2 labelPos = ImGui::GetCursorPos() + ImGui::GetWindowPos();
        labelPos.y -= ImGui::GetTextLineHeight();
        auto size = ImGui::CalcTextSize(label.data(), label.data() + label.size());

        auto padding = ImGui::GetStyle().FramePadding;
        auto spacing = ImGui::GetStyle().ItemSpacing;

        labelPos += ImVec2(spacing.x, -spacing.y);

        auto rectMin = labelPos - padding;
        auto rectMax = labelPos + size + padding;

        auto drawList = ImGui::GetWindowDrawList();
        //drawList->ChannelsSetCurrent(3);
        drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
        //drawList->ChannelsSetCurrent(4);
        drawList->AddText(labelPos, IM_COL32(255, 255, 255, 255), label.data(), label.data() + label.size());
    };

    ImColor DataColor(uint32_t mask, ExtendedValueTypeDesc type)
    {
        if (type.mType == ValueTypeEnum::SenderValue) {
            return FlowColor(mask);
        }
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

    ImVec2 DataPinIcon(ExtendedValueTypeDesc type, uint32_t mask, bool connected)
    {
        ax::Widgets::IconType icon = ax::Widgets::IconType::Circle;
        if (type.mType == ValueTypeEnum::SenderValue)
            icon = ax::Widgets::IconType::Diamond;

        ax::Widgets::Icon(ImVec2(sPinIconSize, sPinIconSize), icon, connected, DataColor(mask, type), ImColor(32, 32, 32, 255));
        ImVec2 align { 0.5f, 0.5f };
        return ImGui::GetItemRectMin() + align * ImGui::GetItemRectSize();
    }

    ImVec2 DataInstancePinIcon(ExtendedValueTypeDesc type, uint32_t mask, bool connected)
    {
        ax::Widgets::IconType icon = ax::Widgets::IconType::Square;
        if (type.mType == ValueTypeEnum::SenderValue)
            icon = ax::Widgets::IconType::Diamond;

        ax::Widgets::Icon(ImVec2(sPinIconSize, sPinIconSize), icon, connected, DataColor(mask, type), ImColor(32, 32, 32, 255));
        ImVec2 align { 0.5f, 0.5f };
        return ImGui::GetItemRectMin() + align * ImGui::GetItemRectSize();
    }

    ImVec2 FlowPinIcon(uint32_t mask, bool connected)
    {
        ax::Widgets::Icon(ImVec2(sPinIconSize, sPinIconSize), ax::Widgets::IconType::Flow, connected, FlowColor(mask), ImColor(32, 32, 32, 255));
        ImVec2 align { 0.5f, 0.5f };
        return ImGui::GetItemRectMin() + align * ImGui::GetItemRectSize();
    }

    ImVec2 FlowOutPin(const char *name, uint32_t mask, bool connected)
    {
        float textSize = name ? ImGui::CalcTextSize(name).x : 0.0f;
        ImGui::RightAlign(textSize + sPinIconSize + 8);

        if (name) {
            ImGui::Text("%s", name);
            ImGui::SameLine();
        }
        return FlowPinIcon(mask, connected);
    }

    void FlowOutPin(const char *name, uint32_t nodeId, uint32_t pinId, uint32_t group, uint32_t mask, bool connected)
    {
        ed::BeginPin(60000 * nodeId + NodeGraph::NodeBase::flowOutId(pinId, group), ed::PinKind::Output);
        ImVec2 pos = FlowOutPin(name, mask, connected);
        ed::PinPivotRect(pos, pos);
        ed::EndPin();
    }

    ImVec2 FlowInPin(const char *name, uint32_t mask, bool connected)
    {
        ImVec2 pos = FlowPinIcon(mask, connected);
        if (name) {
            ImGui::SameLine();
            ImGui::Text("%s", name);
        }
        return pos;
    }

    void FlowInPin(const char *name, uint32_t nodeId, uint32_t pinId, uint32_t group, uint32_t mask, bool connected)
    {
        ed::BeginPin(60000 * nodeId + NodeGraph::NodeBase::flowInId(pinId, group), ed::PinKind::Input);
        ImVec2 pos = FlowInPin(name, mask, connected);
        ed::PinPivotRect(pos, pos);
        ed::EndPin();
    }

    ImVec2 DataOutPin(const char *name, ExtendedValueTypeDesc type, uint32_t mask, bool connected)
    {
        float textSize = name ? ImGui::CalcTextSize(name).x : 0.0f;
        ImGui::RightAlign(textSize + sPinIconSize + 8);
        if (name) {
            ImGui::Text("%s", name);
            ImGui::SameLine();
        }
        return DataPinIcon(type, mask, connected);
    }

    bool DataOutPin(const char *name, uint32_t nodeId, uint32_t pinId, uint32_t group, ExtendedValueTypeDesc type, uint32_t mask, bool connected)
    {
        ed::BeginPin(60000 * nodeId + NodeGraph::NodeBase::dataOutId(pinId, group), ed::PinKind::Output);
        ImVec2 pos = DataOutPin(name, type, mask, connected);
        ed::PinPivotRect(pos, pos);
        ed::EndPin();
        return ImGui::IsItemHovered();
    }

    ImVec2 DataInPin(const char *name, ExtendedValueTypeDesc type, uint32_t mask, bool connected)
    {
        ImVec2 pos = DataPinIcon(type, mask, connected);
        if (name) {
            ImGui::SameLine();
            ImGui::Text("%s", name);
        }
        return pos;
    }

    bool DataInPin(const char *name, uint32_t nodeId, uint32_t pinId, uint32_t group, ExtendedValueTypeDesc type, uint32_t mask, bool connected)
    {
        uintptr_t id = 60000 * nodeId + NodeGraph::NodeBase::dataInId(pinId, group);
        ed::BeginPin(id, ed::PinKind::Input);
        ImVec2 pos = DataInPin(name, type, mask, connected);
        ed::PinPivotRect(pos, pos);
        ed::EndPin();
        return ImGui::IsItemHovered();
    }

    ImVec2 DataProviderPin(const char *name, ExtendedValueTypeDesc type, uint32_t mask, bool connected)
    {
        float textSize = name ? ImGui::CalcTextSize(name).x : 0.0f;
        ImGui::RightAlign(textSize + sPinIconSize + 8);
        if (name) {
            ImGui::Text("%s", name);
            ImGui::SameLine();
        }
        return DataInstancePinIcon(type, mask, connected);
    }

    bool DataProviderPin(const char *name, uint32_t nodeId, uint32_t pinId, uint32_t group, ExtendedValueTypeDesc type, uint32_t mask, bool connected)
    {
        ed::BeginPin(60000 * nodeId + NodeGraph::NodeBase::dataProviderId(pinId, group), ed::PinKind::Output);
        ImVec2 pos = DataProviderPin(name, type, mask, connected);
        ed::PinPivotRect(pos, pos);
        ed::EndPin();
        return ImGui::IsItemHovered();
    }

    ImVec2 DataReceiverPin(const char *name, ExtendedValueTypeDesc type, uint32_t mask, bool connected)
    {
        ImVec2 pos = DataInstancePinIcon(type, mask, connected);
        if (name) {
            ImGui::SameLine();
            ImGui::Text("%s", name);
        }
        return pos;
    }

    bool DataReceiverPin(const char *name, size_t nodeId, size_t pinId, uint32_t group, ExtendedValueTypeDesc type, uint32_t mask, bool connected)
    {
        ed::BeginPin(60000 * nodeId + NodeGraph::NodeBase::dataReceiverId(pinId, group), ed::PinKind::Input);
        ImVec2 pos = DataReceiverPin(name, type, mask, connected);
        ed::PinPivotRect(pos, pos);
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

    bool BeginNode(const NodeGraph::NodeBase *node)
    {
        ImGui::Text(node->name());

        std::string id = std::to_string(reinterpret_cast<uintptr_t>(node));
        return ImGui::BeginTable(id.c_str(), 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX);
    }

    void EndNode()
    {
        ImGui::EndTable();
    }


}
}
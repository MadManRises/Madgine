#include "../nodegraphtoolslib.h"

#include "debugvisualizer.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imguiaddons.h"
#include "imguihelpers.h"

#include "Madgine/nodegraph/pins.h"

#include "Madgine/nodegraph/nodeinterpreter.h"

#include "Madgine_Tools/debugger/debuggerview.h"

#include "imguihelpers.h"

#include "NodeEditor/imgui_node_editor.h"
#include "NodeEditor/imgui_node_editor_internal.h"

#include "Generic/projections.h"

namespace Engine {
namespace Tools {

    struct ConnectorInfo {
        ImVec2 mPos;
        NodeGraph::PinType mType;
        uint32_t mIndex;
    };

    const NodeGraph::NodeDebugLocation *visualizeDebugLocation(DebuggerView *view, const Debug::ContextInfo *context, const NodeGraph::NodeDebugLocation *location, const Debug::DebugLocation *inlineLocation)
    {
        const Debug::DebugLocation *child = nullptr;
        const NodeGraph::NodeGraph &graph = *location->mInterpreter->graph();

        if (inlineLocation) {

            const NodeGraph::NodeDebugLocation *inlineNodeLocation = dynamic_cast<const NodeGraph::NodeDebugLocation *>(inlineLocation);
            if (!inlineNodeLocation || inlineNodeLocation->mInterpreter != location->mInterpreter)
                return location;

            ImGui::EndVertical();

            EndNode();

            BeginNode(location->mNode, graph.nodeIndex(location->mNode));

            ImGui::BeginVertical("child");

            child = view->visualizeDebugLocation(context, location->mChild, inlineLocation);

            ed::SelectNode(60000 * graph.nodeIndex(location->mNode), true);
        } else {
            if (!location->mEditorContext) {
                ed::Config config;

                config.UserPointer = location->mInterpreter;

                config.LoadSettings = [](char *data, void *userPointer) {
                    const std::string &layout = static_cast<NodeGraph::NodeInterpreterStateBase *>(userPointer)->graph()->mLayoutData;
                    if (data) {
                        strcpy(data, layout.c_str());
                    }
                    return layout.size();
                };

                location->mEditorContext = { ed::CreateEditor(&config), &ed::DestroyEditor };
            }

            ImGui::PushID(location);

            ImRect oldViewport = BeginNodeEditor(location->mEditorContext.get(), { 0, 250 });

            ed::ClearSelection();

            if (location->mNode) {
                uint32_t nodeId = graph.nodeIndex(location->mNode);
                BeginNode(location->mNode, nodeId);

                ImGui::BeginVertical("child");
                child = view->visualizeDebugLocation(context, location->mChild, location);
                ImGui::EndVertical();

                EndNode();

                ed::SelectNode(60000 * nodeId, true);
            }

            ed::NodeId ids[256];
            auto result = ed::GetOrderedNodeIds(ids, 256);
            assert(result < 255);
            uint32_t handledIndex = 0;

            uint32_t nodeId = 1;
            for (NodeGraph::NodeBase *node : graph.nodes() | std::views::transform(projectionUniquePtrToPtr)) {

                if (static_cast<size_t>(ids[handledIndex]) == 60000 * nodeId) {
                    ++handledIndex;
                } else {
                    BeginNode(node, nodeId);

                    EndNode();
                }

                ++nodeId;
            }

            nodeId = 1;
            for (NodeGraph::NodeBase *node : graph.nodes() | std::views::transform(projectionUniquePtrToPtr)) {
                NodeLinks(node, nodeId);
                ++nodeId;
            }

            ed::NavigateToSelection(true);

            EndNodeEditor(oldViewport);

            ImGui::PopID();
        }

        if (child)
            view->visualizeDebugLocation(context, child, nullptr);

        return nullptr;
    }

}
}
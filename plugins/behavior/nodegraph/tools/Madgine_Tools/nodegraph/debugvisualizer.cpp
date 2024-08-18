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

#include "NodeEditor/imgui_node_editor_internal.h"
#include "NodeEditor/imgui_node_editor.h"


#include "Generic/projections.h"

namespace Engine {
namespace Tools {

    struct ConnectorInfo {
        ImVec2 mPos;
        NodeGraph::PinType mType;
        uint32_t mIndex;
    };

    const NodeGraph::NodeDebugLocation *visualizeDebugLocation(DebuggerView *view, const Debug::ContextInfo *context, const NodeGraph::NodeDebugLocation *location, bool isInline)
    {
        if (isInline)
            return location;

        const NodeGraph::NodeGraph &graph = *location->mInterpreter->graph();

        const Debug::DebugLocation *child = nullptr;

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

        ImRect oldViewport = BeginNodeEditor(location->mEditorContext.get(), { 0, 150 });

        uint32_t nodeId = 1;
        NodeGraph::NodeBase *hoveredNode = nullptr;
        for (NodeGraph::NodeBase *node : graph.nodes() | std::views::transform(projectionUniquePtrToPtr)) {

            BeginNode(node, nodeId);                

            if (node == location->mNode) {
                ImGui::BeginVertical("child");                
                child = view->visualizeDebugLocation(context, location->mChild, true);                
                ImGui::EndVertical();
                ed::SelectNode(60000 * nodeId);
                ed::NavigateToSelection(true);                
            }

            EndNode();

            if (ImGui::IsItemHovered()) {
                hoveredNode = node;
            }

            ++nodeId;
        }

        nodeId = 1;
        for (NodeGraph::NodeBase *node : graph.nodes() | std::views::transform(projectionUniquePtrToPtr)) {
            NodeLinks(node, nodeId);
            ++nodeId;
        }

        EndNodeEditor(oldViewport);

        ImGui::PopID();

        if (child)
            view->visualizeDebugLocation(context, child, false);

        return nullptr;
    }

}
}
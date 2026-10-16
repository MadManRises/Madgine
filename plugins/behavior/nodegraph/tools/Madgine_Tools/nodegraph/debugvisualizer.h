#pragma once

namespace Engine {
namespace Tools {

    std::vector<TypedPtr> visualizeDebugLocation(ContinuationList &continuations, DebuggerView &view, const Behavior::NodeGraph::NodeDebugLocation *location, TypedPtr inlineLocation);

}
}
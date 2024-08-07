#pragma once

#include "../../nodebase.h"
#include "../../nodecollector.h"

#include "Madgine/parametertuple.h"

#include "Madgine/behaviorcollector.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT LibraryNode : Serialize::VirtualData<LibraryNode, VirtualScope<LibraryNode, NodeBase>> {

        LibraryNode(NodeGraph &graph, BehaviorHandle behavior);
        LibraryNode(const LibraryNode &other, NodeGraph &graph);

        std::string_view name() const override;
        std::string_view className() const override;
        std::unique_ptr<NodeBase> clone(NodeGraph &graph) const override;

        uint32_t flowInCount(uint32_t group) const override;

        uint32_t flowOutBaseCount(uint32_t group) const override;

        uint32_t dataProviderBaseCount(uint32_t group) const override;
        ExtendedValueTypeDesc dataProviderType(uint32_t index, uint32_t group = 0, bool bidir = true) const override;

        void setupInterpret(NodeInterpreterStateBase &interpreter, std::unique_ptr<NodeInterpreterData> &data) const override;
        void interpret(NodeReceiver<NodeBase> receiver, std::unique_ptr<NodeInterpreterData> &data, uint32_t flowIn, uint32_t group) const override;
        void interpretRead(NodeInterpreterStateBase &interpreter, ValueType &retVal, std::unique_ptr<NodeInterpreterData> &data, uint32_t providerIndex, uint32_t group = 0) const override;

        ParameterTuple mParameters;

    private:
        BehaviorHandle mBehavior;        
        std::string mFullClassName;
    };

}
}

REGISTER_TYPE(Engine::NodeGraph::LibraryNode)
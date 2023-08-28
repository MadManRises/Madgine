#pragma once

#include "Madgine/nodegraph/nodebase.h"
#include "Madgine/nodegraph/nodecollector.h"

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_NODES_EXPORT RenderPassNode : NodeGraph::Node<RenderPassNode> {

        RenderPassNode(NodeGraph::NodeGraph &graph);
        RenderPassNode(const RenderPassNode &other, NodeGraph::NodeGraph &graph);

        //virtual std::string_view name() const override;

        virtual size_t flowOutGroupCount() const override;
        virtual size_t flowOutBaseCount(uint32_t group = 0) const override;
        virtual std::string_view flowOutName(uint32_t index, uint32_t group = 0) const override;
 
        virtual size_t dataProviderGroupCount() const override;
        virtual size_t dataProviderBaseCount(uint32_t group = 0) const override;
        virtual std::string_view dataProviderName(uint32_t index, uint32_t group = 0) const override;
        virtual ExtendedValueTypeDesc dataProviderType(uint32_t index, uint32_t group = 0, bool bidir = true) const override;

        virtual void interpret(NodeGraph::NodeReceiver<NodeBase> receiver, std::unique_ptr<NodeGraph::NodeInterpreterData> &data, uint32_t flowIn, uint32_t group) const override;
        virtual void interpretRead(NodeGraph::NodeInterpreter &interpreter, ValueType &retVal, std::unique_ptr<NodeGraph::NodeInterpreterData> &data, uint32_t providerIndex, uint32_t group = 0) const override;
    };

}
}

REGISTER_TYPE(Engine::Render::RenderPassNode)

#pragma once

#include "Madgine/nodegraph/nodebase.h"
#include "Madgine/nodegraph/nodecollector.h"

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_NODES_EXPORT FrameBufferNode : NodeGraph::Node<FrameBufferNode> {

        FrameBufferNode(NodeGraph::NodeGraph &graph);
        FrameBufferNode(const FrameBufferNode &other, NodeGraph::NodeGraph &graph);

        //virtual std::string_view name() const override;

        virtual size_t flowInCount(uint32_t group) const override;
        virtual std::string_view flowInName(uint32_t index, uint32_t group) const override;
        virtual uint32_t flowInMask(uint32_t index, uint32_t group, bool bidir = true) const override;

        virtual size_t dataInBaseCount(uint32_t group) const override;
        virtual std::string_view dataInName(uint32_t index, uint32_t group) const override;
        virtual ExtendedValueTypeDesc dataInType(uint32_t index, uint32_t group, bool bidir = true) const override;
        virtual uint32_t dataInMask(uint32_t index, uint32_t group, bool bidir = true) const override;

        virtual void interpret(NodeGraph::NodeReceiver<NodeBase> receiver, std::unique_ptr<NodeGraph::NodeInterpreterData> &data, uint32_t flowIn, uint32_t group) const override;

        virtual void generate(NodeGraph::CodeGenerator &generator, std::unique_ptr<NodeGraph::CodeGeneratorData> &data, uint32_t flowIn, uint32_t group = 0) const override;
    };

}
}

REGISTER_TYPE(Engine::Render::FrameBufferNode)

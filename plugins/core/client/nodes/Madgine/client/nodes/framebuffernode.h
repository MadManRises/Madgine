#pragma once

#include "Madgine/nodegraph/nodebase.h"
#include "Madgine/nodegraph/nodecollector.h"

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_NODES_EXPORT FrameBufferNode : NodeGraph::Node<FrameBufferNode> {

        FrameBufferNode(NodeGraph::NodeGraph &graph);
        FrameBufferNode(const FrameBufferNode &other, NodeGraph::NodeGraph &graph);

        //virtual std::string_view name() const override;

        virtual size_t flowInCount() const override;
        virtual std::string_view flowInName(uint32_t index) const override;
        virtual uint32_t flowInMask(uint32_t index, bool bidir = true) const override;

        virtual size_t dataInCount() const override;
        virtual std::string_view dataInName(uint32_t index) const override;
        virtual ExtendedValueTypeDesc dataInType(uint32_t index, bool bidir = true) const override;
        virtual uint32_t dataInMask(uint32_t index, bool bidir = true) const override;

        virtual void interpret(NodeGraph::NodeReceiver receiver, uint32_t flowIn, std::unique_ptr<NodeGraph::NodeInterpreterData> &data) const override;

        virtual void generate(NodeGraph::CodeGenerator &generator, IndexType<uint32_t> &flowInOut, std::unique_ptr<NodeGraph::CodeGeneratorData> &data) const override;
    };

}
}

REGISTER_TYPE(Engine::Render::FrameBufferNode)

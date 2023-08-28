#pragma once

#include "Madgine/nodegraph/nodebase.h"
#include "Madgine/nodegraph/nodecollector.h"

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_NODES_EXPORT MeshRendererNode : NodeGraph::Node<MeshRendererNode> {

        MeshRendererNode(NodeGraph::NodeGraph &graph);
        MeshRendererNode(const MeshRendererNode &other, NodeGraph::NodeGraph &graph);

        //virtual std::string_view name() const override;

        virtual size_t flowInCount(uint32_t group) const override;
        virtual std::string_view flowInName(uint32_t index, uint32_t group) const override;

        virtual size_t flowOutBaseCount(uint32_t group) const override;
        virtual std::string_view flowOutName(uint32_t index, uint32_t group) const override;

        virtual size_t dataInBaseCount(uint32_t group) const override;
        virtual std::string_view dataInName(uint32_t index, uint32_t group) const override;
        virtual ExtendedValueTypeDesc dataInType(uint32_t index, uint32_t group, bool bidir = true) const override;

        virtual void interpret(NodeGraph::NodeReceiver<NodeGraph::NodeBase> receiver, std::unique_ptr<NodeGraph::NodeInterpreterData> &data, uint32_t flowIn, uint32_t group) const override;

        virtual void generate(NodeGraph::CodeGenerator &generator, std::unique_ptr<NodeGraph::CodeGeneratorData> &data, uint32_t flowIn, uint32_t group) const override;
    };

}
}

REGISTER_TYPE(Engine::Render::MeshRendererNode)

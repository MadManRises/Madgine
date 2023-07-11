#pragma once

#include "Madgine/nodegraph/nodebase.h"
#include "Madgine/nodegraph/nodecollector.h"

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_NODES_EXPORT RenderTextureNode : NodeGraph::Node<RenderTextureNode> {

        RenderTextureNode(NodeGraph::NodeGraph &graph);
        RenderTextureNode(const RenderTextureNode &other, NodeGraph::NodeGraph &graph);

        //virtual std::string_view name() const override;

        virtual size_t dataProviderBaseCount(uint32_t group = 0) const override;
        virtual std::string_view dataProviderName(uint32_t index, uint32_t group = 0) const override;
        virtual ExtendedValueTypeDesc dataProviderType(uint32_t index, uint32_t group = 0, bool bidir = true) const override;

        virtual bool dataInVariadic(uint32_t group = 0) const override;
        virtual std::string_view dataInName(uint32_t index, uint32_t group = 0) const override;
        virtual ExtendedValueTypeDesc dataInType(uint32_t index, uint32_t group = 0, bool bidir = true) const override;

        virtual void interpret(NodeGraph::NodeReceiver receiver, std::unique_ptr<NodeGraph::NodeInterpreterData> &data, uint32_t flowIn, uint32_t group) const override;
    };

}
}

REGISTER_TYPE(Engine::Render::RenderTextureNode)

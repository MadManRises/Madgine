#pragma once

#include "Madgine/nodegraph/nodebase.h"
#include "Madgine/nodegraph/nodecollector.h"

namespace Engine {
namespace Scene {

    struct MADGINE_SCENE_NODES_EXPORT TransformAccessorNode : NodeGraph::Node<TransformAccessorNode> {

        using Node::Node;

        virtual size_t dataProviderBaseCount(uint32_t group) const override;
        virtual std::string_view dataProviderName(uint32_t index, uint32_t group) const override;
        virtual ExtendedValueTypeDesc dataProviderType(uint32_t index, uint32_t group,bool bidir = true) const override;

        virtual size_t dataReceiverBaseCount(uint32_t group) const override;
        virtual std::string_view dataReceiverName(uint32_t index, uint32_t group) const override;
        virtual ExtendedValueTypeDesc dataReceiverType(uint32_t index, uint32_t group, bool bidir = true) const override;

        virtual void interpretRead(NodeGraph::NodeInterpreter &interpreter, ValueType &retVal, std::unique_ptr<NodeGraph::NodeInterpreterData> &data, uint32_t providerIndex, uint32_t group) const override;
        virtual void interpretWrite(NodeGraph::NodeInterpreter &interpreter, std::unique_ptr<NodeGraph::NodeInterpreterData> &data, const ValueType &v, uint32_t receiverIndex, uint32_t group) const override;

    };

}
}

REGISTER_TYPE(Engine::Scene::TransformAccessorNode)

#pragma once

#include "Madgine/nodegraph/nodebase.h"
#include "Madgine/nodegraph/nodecollector.h"

namespace Engine {
namespace Scene {

    struct MADGINE_SCENE_EXPORT TransformAccessorNode : NodeGraph::Node<TransformAccessorNode> {

        TransformAccessorNode(NodeGraph::NodeGraph &graph);

        virtual size_t dataProviderCount() const override;
        virtual std::string_view dataProviderName(uint32_t index) const override;
        virtual ExtendedValueTypeDesc dataProviderType(uint32_t index) const override;

        virtual size_t dataReceiverCount() const override;
        virtual std::string_view dataReceiverName(uint32_t index) const override;
        virtual ExtendedValueTypeDesc dataReceiverType(uint32_t index) const override;

        virtual ValueType interpretRead(NodeGraph::NodeInterpreter &interpreter, uint32_t providerIndex, std::unique_ptr<NodeGraph::NodeInterpreterData> &data) const override;
        virtual void interpretWrite(NodeGraph::NodeInterpreter &interpreter, uint32_t receiverIndex, std::unique_ptr<NodeGraph::NodeInterpreterData> &data, const ValueType &v) const override;

    };

}
}

RegisterType(Engine::Scene::TransformAccessorNode);
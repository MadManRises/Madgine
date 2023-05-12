#pragma once

#include "../nodebase.h"
#include "../nodecollector.h"

#include "util/automasknode.h"


namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT PumpNode : Node<PumpNode, AutoMaskNode<>> {

        PumpNode(NodeGraph &graph);
        PumpNode(const PumpNode &other, NodeGraph &graph);

        virtual size_t flowInCount(uint32_t group) const override;

        virtual size_t flowOutBaseCount(uint32_t group) const override;

        virtual size_t dataInBaseCount(uint32_t group) const override;
        virtual ExtendedValueTypeDesc dataInType(uint32_t index, uint32_t group, bool bidir = true) const override;

        virtual size_t dataOutBaseCount(uint32_t group) const override;
        virtual ExtendedValueTypeDesc dataOutType(uint32_t index, uint32_t group, bool bidir = true) const override;

        virtual void interpret(NodeReceiver receiver, std::unique_ptr<NodeInterpreterData> &data, uint32_t flowIn, uint32_t group) const override;
    };

}
}

REGISTER_TYPE(Engine::NodeGraph::PumpNode)

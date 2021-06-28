#pragma once

#include "../nodebase.h"
#include "../nodecollector.h"

#include "util/automasknode.h"


namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT PumpNode : Node<PumpNode, AutoMaskNode<>> {

        PumpNode(NodeGraph &graph);
        PumpNode(const PumpNode &other, NodeGraph &graph);

        virtual size_t flowInCount() const override;
        virtual std::string_view flowInName(uint32_t index) const override;

        virtual size_t flowOutCount() const override;
        virtual std::string_view flowOutName(uint32_t index) const override;

        virtual size_t dataInCount() const override;
        virtual std::string_view dataInName(uint32_t index) const override;
        virtual ExtendedValueTypeDesc dataInType(uint32_t index, bool bidir = true) const override;

        virtual size_t dataOutCount() const override;
        virtual std::string_view dataOutName(uint32_t index) const override;
        virtual ExtendedValueTypeDesc dataOutType(uint32_t index, bool bidir = true) const override;

        virtual void interpret(NodeInterpreter &interpreter, IndexType<uint32_t> &flowInOut, std::unique_ptr<NodeInterpreterData> &data) const override;
    };

}
}

RegisterType(Engine::NodeGraph::PumpNode);

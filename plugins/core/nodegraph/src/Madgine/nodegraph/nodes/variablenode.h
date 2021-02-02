#pragma once

#include "../nodebase.h"
#include "../nodecollector.h"

#include "Modules/keyvalue/valuetype.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT VariableNode : Node<VariableNode> {

        VariableNode(NodeGraph &graph);

        virtual size_t dataProviderCount() const override;
        virtual std::string_view dataProviderName(uint32_t index) const override;
        virtual ExtendedValueTypeDesc dataProviderType(uint32_t index) const override;

        virtual size_t dataReceiverCount() const override;
        virtual std::string_view dataReceiverName(uint32_t index) const override;
        virtual ExtendedValueTypeDesc dataReceiverType(uint32_t index) const override;


        virtual ValueType interpretRead(NodeInterpreter &interpreter, uint32_t providerIndex, std::unique_ptr<NodeInterpreterData> &data) const override;
        virtual void interpretWrite(NodeInterpreter &interpreter, uint32_t receiverIndex, std::unique_ptr<NodeInterpreterData> &data, const ValueType &v) const override;


        ValueType mDefaultValue;
    };

}
}

RegisterType(Engine::NodeGraph::VariableNode);

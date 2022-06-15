#pragma once

#include "../nodebase.h"
#include "../nodecollector.h"

#include "Meta/keyvalue/valuetype.h"

#include "util/automasknode.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT VariableNode : Node<VariableNode, AutoMaskNode<>> {

        VariableNode(NodeGraph &graph);
        VariableNode(const VariableNode &other, NodeGraph &graph);

        virtual size_t dataProviderCount() const override;
        virtual std::string_view dataProviderName(uint32_t index) const override;
        virtual ExtendedValueTypeDesc dataProviderType(uint32_t index, bool bidir = true) const override;

        virtual size_t dataReceiverCount() const override;
        virtual std::string_view dataReceiverName(uint32_t index) const override;
        virtual ExtendedValueTypeDesc dataReceiverType(uint32_t index, bool bidir = true) const override;


        virtual void interpretRead(NodeInterpreter &interpreter, ValueType &retVal, uint32_t providerIndex, std::unique_ptr<NodeInterpreterData> &data) const override;
        virtual void interpretWrite(NodeInterpreter &interpreter, uint32_t receiverIndex, std::unique_ptr<NodeInterpreterData> &data, const ValueType &v) const override;

        virtual CodeGen::Statement generateRead(CodeGenerator &generator, uint32_t providerIndex, std::unique_ptr<CodeGeneratorData> &data) const override;

        ValueType mDefaultValue;
    };

}
}

REGISTER_TYPE(Engine::NodeGraph::VariableNode)

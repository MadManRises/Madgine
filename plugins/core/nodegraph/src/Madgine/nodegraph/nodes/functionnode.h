#pragma once

#include "../nodebase.h"
#include "../nodecollector.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT FunctionNode : Node<FunctionNode> {

        FunctionNode(NodeGraph &graph);
        FunctionNode(const FunctionNode &other, NodeGraph &graph);

        virtual size_t dataInBaseCount(uint32_t group = 0) const override;
        virtual std::string_view dataInName(uint32_t index, uint32_t group) const override;
        virtual ExtendedValueTypeDesc dataInType(uint32_t index, uint32_t group, bool bidir = true) const override;

        virtual size_t dataProviderBaseCount(uint32_t group = 0) const override;
        virtual ExtendedValueTypeDesc dataProviderType(uint32_t index, uint32_t group, bool bidir = true) const override;

        virtual void interpretRead(NodeInterpreter &interpreter, ValueType &retVal, std::unique_ptr<NodeInterpreterData> &data, uint32_t providerIndex, uint32_t group) const override;
    
        virtual CodeGen::Statement generateRead(CodeGenerator &generator, std::unique_ptr<CodeGeneratorData> &data, uint32_t providerIndex, uint32_t group = 0) const override;

        void setFunction(FunctionTable *function);
        FunctionTable *getFunction() const;

        std::string_view getFunctionName() const;
        void setFunctionName(std::string_view name);

        FunctionTable *mFunction = nullptr;
    };

}
}

REGISTER_TYPE(Engine::NodeGraph::FunctionNode)

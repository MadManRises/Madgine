#pragma once

#include "../nodebase.h"
#include "../nodecollector.h"

#include "Meta/keyvalue/valuetype.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT FunctionNode : Node<FunctionNode> {

        FunctionNode(NodeGraph &graph);
        FunctionNode(const FunctionNode &other, NodeGraph &graph);

        virtual size_t dataInCount() const override;
        virtual std::string_view dataInName(uint32_t index) const override;
        virtual ExtendedValueTypeDesc dataInType(uint32_t index, bool bidir = true) const override;

        virtual size_t dataProviderCount() const override;
        virtual std::string_view dataProviderName(uint32_t index) const override;
        virtual ExtendedValueTypeDesc dataProviderType(uint32_t index, bool bidir = true) const override;

        virtual void interpretRead(NodeInterpreter &interpreter, ValueType &retVal, uint32_t providerIndex, std::unique_ptr<NodeInterpreterData> &data) const override;
    
        virtual CodeGen::Statement generateRead(CodeGenerator &generator, uint32_t providerIndex, std::unique_ptr<CodeGeneratorData> &data) const override;

        void setFunction(FunctionTable *function);
        FunctionTable *getFunction() const;

        std::string_view getFunctionName() const;
        void setFunctionName(std::string_view name);

        FunctionTable *mFunction = nullptr;
    };

}
}

RegisterType(Engine::NodeGraph::FunctionNode);

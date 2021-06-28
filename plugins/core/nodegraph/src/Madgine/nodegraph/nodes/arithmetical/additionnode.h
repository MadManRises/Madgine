#pragma once

#include "../util/automasknode.h"
#include "../../nodecollector.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT AdditionNode : Node<AdditionNode, AutoMaskNode<>> {
        
        AdditionNode(NodeGraph &graph);
        AdditionNode(const AdditionNode &other, NodeGraph &graph);

        virtual size_t dataInCount() const override;
        virtual ExtendedValueTypeDesc dataInType(uint32_t index, bool bidir = true) const override;
        virtual std::string_view dataInName(uint32_t index) const override;

        virtual size_t dataProviderCount() const override;
        virtual ExtendedValueTypeDesc dataProviderType(uint32_t index, bool bidir = true) const override;
        virtual std::string_view dataProviderName(uint32_t index) const override;

        virtual CodeGen::Statement generateRead(CodeGenerator &generator, uint32_t providerIndex, std::unique_ptr<CodeGeneratorData> &data) const override;

    };

}

}

RegisterType(Engine::NodeGraph::AdditionNode)
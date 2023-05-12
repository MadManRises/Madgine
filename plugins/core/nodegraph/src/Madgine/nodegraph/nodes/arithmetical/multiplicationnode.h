#pragma once

#include "../util/automasknode.h"
#include "../../nodecollector.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT MultiplicationNode : Node<MultiplicationNode, AutoMaskNode<>> {
        
        MultiplicationNode(NodeGraph &graph);
        MultiplicationNode(const MultiplicationNode &other, NodeGraph &graph);

        virtual size_t dataInBaseCount(uint32_t group) const override;
        virtual ExtendedValueTypeDesc dataInType(uint32_t index, uint32_t group,bool bidir = true) const override;
        virtual std::string_view dataInName(uint32_t index, uint32_t group) const override;

        virtual size_t dataProviderBaseCount(uint32_t group) const override;
        virtual ExtendedValueTypeDesc dataProviderType(uint32_t index, uint32_t group, bool bidir = true) const override;
        virtual std::string_view dataProviderName(uint32_t index, uint32_t group) const override;

        virtual CodeGen::Statement generateRead(CodeGenerator &generator, uint32_t providerIndex, std::unique_ptr<CodeGeneratorData> &data) const override;

    };

}

}

REGISTER_TYPE(Engine::NodeGraph::MultiplicationNode)
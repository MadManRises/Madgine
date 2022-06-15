#pragma once

#include "Madgine/nodegraph/nodebase.h"
#include "Madgine/nodegraph/nodecollector.h"

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_NODES_EXPORT RasterizerNode : NodeGraph::Node<RasterizerNode> {

        RasterizerNode(NodeGraph::NodeGraph &graph);
        RasterizerNode(const RasterizerNode &other, NodeGraph::NodeGraph &graph);

        //virtual std::string_view name() const override;

        virtual size_t flowInCount() const override;
        virtual std::string_view flowInName(uint32_t index) const override;
        virtual uint32_t flowInMask(uint32_t index, bool bidir = true) const override;

        virtual size_t flowOutCount() const override;
        virtual std::string_view flowOutName(uint32_t index) const override;
        virtual uint32_t flowOutMask(uint32_t index, bool bidir = true) const override;

        virtual size_t dataProviderCount() const override;
        virtual std::string_view dataProviderName(uint32_t index) const override;
        virtual ExtendedValueTypeDesc dataProviderType(uint32_t index, bool bidir = true) const override;
        virtual uint32_t dataProviderMask(uint32_t index, bool bidir = true) const override;

        virtual size_t dataInCount() const override;
        virtual std::string_view dataInName(uint32_t index) const override;
        virtual ExtendedValueTypeDesc dataInType(uint32_t index, bool bidir = true) const override;
        virtual uint32_t dataInMask(uint32_t index, bool bidir = true) const override;
        virtual bool dataInVariadic() const override;

        virtual void onDataInUpdate(uint32_t index, NodeGraph::Pin source, NodeGraph::EdgeEvent event) override;

        virtual void interpret(NodeGraph::NodeInterpreter &interpreter, IndexType<uint32_t> &flowInOut, std::unique_ptr<NodeGraph::NodeInterpreterData> &data) const override;
        virtual void generate(NodeGraph::CodeGenerator &generator, IndexType<uint32_t> &flowInOut, std::unique_ptr<NodeGraph::CodeGeneratorData> &data) const override;

        virtual CodeGen::Statement generateRead(NodeGraph::CodeGenerator &generator, uint32_t providerIndex, std::unique_ptr<NodeGraph::CodeGeneratorData> &data) const override;



        uint32_t mAdditionalPins = 0;
    };

}
}

REGISTER_TYPE(Engine::Render::RasterizerNode)

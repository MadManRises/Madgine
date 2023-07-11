#pragma once

#include "Madgine/nodegraph/nodebase.h"
#include "Madgine/nodegraph/nodecollector.h"

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_NODES_EXPORT RenderPipelineNode : NodeGraph::Node<RenderPipelineNode> {

        RenderPipelineNode(NodeGraph::NodeGraph &graph);
        RenderPipelineNode(const RenderPipelineNode &other, NodeGraph::NodeGraph &graph);

        virtual size_t flowOutGroupCount() const override;
        virtual size_t flowOutBaseCount(uint32_t group) const override;
        virtual std::string_view flowOutName(uint32_t index, uint32_t group) const override;
        virtual uint32_t flowOutMask(uint32_t index, uint32_t group, bool bidir = true) const override;

        virtual size_t dataProviderGroupCount() const override;
        virtual bool dataProviderVariadic(uint32_t group) const override;
        virtual size_t dataProviderBaseCount(uint32_t group) const override;
        virtual std::string_view dataProviderName(uint32_t index, uint32_t group) const override;
        virtual ExtendedValueTypeDesc dataProviderType(uint32_t index, uint32_t group, bool bidir = true) const override;
        virtual uint32_t dataProviderMask(uint32_t index, uint32_t group, bool bidir = true) const override;

        virtual void interpretRead(NodeGraph::NodeInterpreter &interpreter, ValueType &retVal, std::unique_ptr<NodeGraph::NodeInterpreterData> &data, uint32_t providerIndex, uint32_t group = 0) const override;

        virtual void generate(NodeGraph::CodeGenerator &generator, std::unique_ptr<NodeGraph::CodeGeneratorData> &data, uint32_t flowIn, uint32_t group) const override;

        virtual CodeGen::Statement generateRead(NodeGraph::CodeGenerator &generator, std::unique_ptr<NodeGraph::CodeGeneratorData> &data, uint32_t providerIndex, uint32_t group = 0) const override;
    };

}
}

REGISTER_TYPE(Engine::Render::RenderPipelineNode)

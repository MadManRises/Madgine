#pragma once

#include "Madgine/nodegraph/nodebase.h"
#include "Madgine/nodegraph/nodecollector.h"

#include "shadercodegenerator.h"

#include "Generic/bytebuffer.h"
#include "Madgine/nodegraph/nodeinterpreter.h"

namespace Engine {
namespace Render {

    
    struct GPUBufferInterpreterData : NodeGraph::NodeInterpreterData {
        WritableByteBuffer mBuffer;
        std::function<WritableByteBuffer()> mMapper;
    };

    struct GPUBufferCodeGeneratorData : ShaderCodeGeneratorData {
        size_t mIndex;
        GPUBufferInterpreterData *mInterpretData;
    };    

    struct MADGINE_CLIENT_NODES_EXPORT GPUBufferNode : NodeGraph::Node<GPUBufferNode> {

        GPUBufferNode(NodeGraph::NodeGraph &graph);
        GPUBufferNode(const GPUBufferNode &other, NodeGraph::NodeGraph &graph);

        //virtual std::string_view name() const override;

        size_t calculateSize() const;

        virtual size_t flowInCount() const override;
        virtual std::string_view flowInName(uint32_t index) const override;

        virtual size_t flowOutCount() const override;
        virtual std::string_view flowOutName(uint32_t index) const override;

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

REGISTER_TYPE(Engine::Render::GPUBufferNode)

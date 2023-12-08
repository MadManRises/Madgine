#include "../../clientnodeslib.h"

#include "renderpipelinenode.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Madgine/nodegraph/nodeinterpreter.h"

#include "Meta/keyvalue/valuetype.h"

#include "Madgine/render/rendertarget.h"

#include "Madgine/pipelineloader/pipelineloader.h"

#include "shadercodegenerator.h"

#include "gpubuffernode.h"

#include "Madgine/meshloader/gpumeshloader.h"

#include "Madgine/nodegraph/nodes/util/sendernode.h"

NODE(RenderPipelineNode, Engine::Render::RenderPipelineNode)

METATABLE_BEGIN_BASE(Engine::Render::RenderPipelineNode, Engine::NodeGraph::NodeBase)
METATABLE_END(Engine::Render::RenderPipelineNode)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Render::RenderPipelineNode, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(Engine::Render::RenderPipelineNode)

namespace Engine {
namespace Render {

    struct Provider {
        std::string_view mName;
        ExtendedValueTypeDesc mType;
    };

    static constexpr std::array<Provider, 8> sProviders {
        { { "pos", toValueTypeDesc<Vector3>() },
            { "w", toValueTypeDesc<float>() },
            { "pos2", toValueTypeDesc<Vector2>() },
            { "normal", toValueTypeDesc<Vector3>() },
            { "color", toValueTypeDesc<Vector4>() },
            { "UV", toValueTypeDesc<Vector2>() },
            { "boneIDs", toValueTypeDesc<Vector4i>() },
            { "weights", toValueTypeDesc<Vector4>() } }
    };

    RenderPipelineNode::RenderPipelineNode(NodeGraph::NodeGraph &graph)
        : Node(graph)
    {
        setup();
    }

    RenderPipelineNode::RenderPipelineNode(const RenderPipelineNode &other, NodeGraph::NodeGraph &graph)
        : Node(other, graph)
    {
    }

    size_t RenderPipelineNode::flowOutGroupCount() const
    {
        return 2;
    }

    size_t RenderPipelineNode::flowOutBaseCount(uint32_t group) const
    {
        return group == 1 ? 1 : 0;
    }

    std::string_view RenderPipelineNode::flowOutName(uint32_t index, uint32_t group) const
    {
        return "vertex";
    }

    uint32_t RenderPipelineNode::flowOutMask(uint32_t index, uint32_t group, bool bidir) const
    {
        return NodeGraph::NodeExecutionMask::GPU;
    }

    size_t RenderPipelineNode::dataProviderGroupCount() const
    {
        return 2;
    }

    size_t RenderPipelineNode::dataProviderBaseCount(uint32_t group) const
    {
        return group == 1 ? sProviders.size() : 1;
    }

    bool RenderPipelineNode::dataProviderVariadic(uint32_t group) const
    {
        return group == 1;
    }

    std::string_view RenderPipelineNode::dataProviderName(uint32_t index, uint32_t group) const
    {
        return group == 1 ? index < sProviders.size() ? sProviders[index].mName : "Buffer" : "Pipeline";
    }

    ExtendedValueTypeDesc RenderPipelineNode::dataProviderType(uint32_t index, uint32_t group, bool bidir) const
    {
        return group == 1 ? index < sProviders.size() ? sProviders[index].mType : toValueTypeDesc<int>() : toValueTypeDesc<Render::PipelineInstance*>();
    }

    uint32_t RenderPipelineNode::dataProviderMask(uint32_t index, uint32_t group, bool bidir) const
    {
        return group == 1 ? NodeGraph::NodeExecutionMask::GPU : NodeGraph::NodeExecutionMask::CPU;
    }

    struct RenderPipelineNodeInterpret : NodeGraph::NodeInterpreterData {
        Render::PipelineLoader::Instance mPipeline;
    };

    void RenderPipelineNode::interpretRead(NodeGraph::NodeInterpreterStateBase &interpreter, ValueType &retVal, std::unique_ptr<NodeGraph::NodeInterpreterData> &data, uint32_t providerIndex, uint32_t group) const
    {
        if (!data) {
            std::unique_ptr<RenderPipelineNodeInterpret> interpret = std::make_unique<RenderPipelineNodeInterpret>();

            ShaderCodeGenerator gen { mGraph };
            gen.mInterpreter = &interpreter;
            NodeGraph::Pin pin = flowOutTarget(0, 1);

            CodeGen::Struct *vertexData = gen.mFile.getStruct("VertexData");
            vertexData->mAnnotations.push_back("semantic");

            for (const Provider &provider : sProviders) {
                vertexData->mVariables.push_back({ { { gen.mFile.mConditionalsBitMask } }, { std::string { provider.mName }, provider.mType } });
            }

            CodeGen::Struct *rasterizerData = gen.mFile.getStruct("RasterizerData");

            gen.mFile.beginFunction("main", { rasterizerData }, { { "IN", vertexData } });
            gen.generate(pin);
            gen.mFile.endFunction();

            std::vector<size_t> bufferSizes;

            for (GPUBufferCodeGeneratorData *buffer : gen.mBuffers) {
                bufferSizes.push_back(buffer->mInterpretData->mBuffer.mSize);
            }

            interpret->mPipeline.createGenerated({ .bufferSizes = std::move(bufferSizes) }, std::move(gen.mFile));

            for (GPUBufferCodeGeneratorData *buffer : gen.mBuffers) {
                buffer->mInterpretData->mMapper = [&pipeline { interpret->mPipeline }, index { buffer->mIndex }]() mutable {
                    WritableByteBuffer buffer;
                    if (pipeline)
                        buffer = pipeline->mapParameters(index);
                    return buffer;
                };

                //auto targetBuffer = buffer->mInterpretData->mMapper();
                //std::memcpy(targetBuffer.mData, buffer->mInterpretData->mBuffer.mData, buffer->mInterpretData->mBuffer.mSize);
            }

            data = std::move(interpret);
        }
        RenderPipelineNodeInterpret *renderData = static_cast<RenderPipelineNodeInterpret *>(data.get());
        assert(group == 0);
        assert(providerIndex == 0);
        retVal = static_cast<Render::PipelineInstance *>(renderData->mPipeline);
    }

    void RenderPipelineNode::generate(NodeGraph::CodeGenerator &generator, std::unique_ptr<NodeGraph::CodeGeneratorData> &data, uint32_t flowIn, uint32_t group) const
    {
        assert(flowIn == 1);
        assert(!data);

        std::unique_ptr<ShaderCodeGeneratorData> _data = std::make_unique<ShaderCodeGeneratorData>();
        _data->mInstanceIndex = 0;
        data = std::move(_data);

    }

    CodeGen::Statement RenderPipelineNode::generateRead(NodeGraph::CodeGenerator &generator, std::unique_ptr<NodeGraph::CodeGeneratorData> &data, uint32_t providerIndex, uint32_t group) const
    {
        return CodeGen::MemberAccess { std::string { sProviders[providerIndex].mName }, CodeGen::VariableAccess { "IN" } };
    }

}
}

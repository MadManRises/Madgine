#include "../../clientnodeslib.h"

#include "meshrenderernode.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Madgine/nodegraph/nodeinterpreter.h"

#include "Meta/keyvalue/valuetype.h"

#include "Madgine/render/rendertarget.h"

#include "programloaderlib.h"
#include "programloader.h"

#include "shadercodegenerator.h"

#include "gpubuffernode.h"

#include "meshloaderlib.h"
#include "gpumeshloader.h"

#include "render/material.h"

NODE(MeshRendererNode, Engine::Render::MeshRendererNode)

METATABLE_BEGIN_BASE(Engine::Render::MeshRendererNode, Engine::NodeGraph::NodeBase)
METATABLE_END(Engine::Render::MeshRendererNode)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Render::MeshRendererNode, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(Engine::Render::MeshRendererNode)

namespace Engine {
namespace Render {

    struct Provider {
        std::string_view mName;
        ExtendedValueTypeDesc mType;
        std::string_view mGuardName;        
    };

    static constexpr std::array<Provider, 7> sProviders {
        { { "pos", toValueTypeDesc<Vector3>(), "HAS_POSITION0" },
            { "pos2", toValueTypeDesc<Vector2>(), "HAS_POSITION1" },
            { "color", toValueTypeDesc<Vector4>(), "HAS_COLOR0" },
            { "normal", toValueTypeDesc<Vector4>(), "HAS_NORMAL0" },
            { "UV", toValueTypeDesc<Vector2>(), "HAS_TEXCOORD0" },
            { "boneIDs", toValueTypeDesc<Vector4i>(), "HAS_BONEINDICES0" },
            { "weights", toValueTypeDesc<Vector4>(), "HAS_WEIGHTS0" } }
    };

    /*std::string_view Python3FunctionNode::name() const
        {
            return mName;
        }*/

    MeshRendererNode::MeshRendererNode(NodeGraph::NodeGraph &graph)
        : Node(graph)
    {
        setup();
    }

    MeshRendererNode::MeshRendererNode(const MeshRendererNode &other, NodeGraph::NodeGraph &graph)
        : Node(other, graph)
    {
    }

    size_t MeshRendererNode::flowInCount() const
    {
        return 1;
    }

    std::string_view MeshRendererNode::flowInName(uint32_t index) const
    {
        return "execute";
    }

    size_t MeshRendererNode::flowOutCount() const
    {
        return 2;
    }

    std::string_view MeshRendererNode::flowOutName(uint32_t index) const
    {
        return index == 0 ? "return" : "vertex";
    }

    uint32_t MeshRendererNode::flowOutMask(uint32_t index, bool bidir) const
    {
        return index == 0 ? NodeGraph::NodeExecutionMask::CPU : NodeGraph::NodeExecutionMask::GPU;
    }

    size_t MeshRendererNode::dataProviderCount() const
    {
        return sProviders.size();
    }

    std::string_view MeshRendererNode::dataProviderName(uint32_t index) const
    {

        return sProviders[index].mName;
    }

    ExtendedValueTypeDesc MeshRendererNode::dataProviderType(uint32_t index, bool bidir) const
    {
        return sProviders[index].mType;
    }

    uint32_t MeshRendererNode::dataProviderMask(uint32_t index, bool bidir) const
    {
        return NodeGraph::NodeExecutionMask::GPU;
    }

    size_t MeshRendererNode::dataInCount() const
    {
        return 2;
    }

    std::string_view MeshRendererNode::dataInName(uint32_t index) const
    {
        return index == 0 ? "renderTarget" : "mesh";
    }

    ExtendedValueTypeDesc MeshRendererNode::dataInType(uint32_t index, bool bidir) const
    {
        return index == 0 ? toValueTypeDesc<Render::RenderTarget *>() : toValueTypeDesc<Render::GPUMeshLoader::ResourceType *>();
    }

    struct MeshRendererNodeInterpret : NodeGraph::NodeInterpreterData {
        Render::ProgramLoader::HandleType mHandle;
        Render::GPUMeshLoader::HandleType mMesh;
    };

    void MeshRendererNode::interpret(NodeGraph::NodeInterpreter &interpreter, IndexType<uint32_t> &flowInOut, std::unique_ptr<NodeGraph::NodeInterpreterData> &data) const
    {

        if (!data) {
            std::unique_ptr<MeshRendererNodeInterpret> interpret = std::make_unique<MeshRendererNodeInterpret>();

            ShaderCodeGenerator gen { mGraph };
            gen.mInterpreter = &interpreter;
            NodeGraph::Pin pin = flowOutTarget(1);

            CodeGen::Struct *vertexData = gen.mFile.getStruct("VertexData");
            vertexData->mAnnotations.push_back("semantic");

            for (const Provider &provider : sProviders) {
                gen.mFile.beginCondition(provider.mGuardName);
                vertexData->mVariables.push_back({ std::string { provider.mName }, provider.mType, gen.mFile.mConditionalsBitMask });
                gen.mFile.endCondition(provider.mGuardName);
            }

            CodeGen::Struct *rasterizerData = gen.mFile.getStruct("RasterizerData");

            gen.mFile.beginFunction("main", rasterizerData, { { "IN", vertexData } });
            IndexType<uint32_t> dummy = 1;
            gen.generate(dummy, this);
            gen.mFile.endFunction();

            interpret->mHandle.createUnnamed(std::move(gen.mFile));

            for (GPUBufferCodeGeneratorData *buffer : gen.mBuffers) {
                throw 0; //??
                //interpret->mHandle.setParametersSize(buffer->mIndex, buffer->mInterpretData->mBuffer.mSize);

                buffer->mInterpretData->mMapper = [handle { interpret->mHandle }, index { buffer->mIndex }]() mutable {
                    return handle.mapParameters(index);
                };

                auto targetBuffer = buffer->mInterpretData->mMapper();
                std::memcpy(targetBuffer.mData, buffer->mInterpretData->mBuffer.mData, buffer->mInterpretData->mBuffer.mSize);
            }

            //interpret->mHandle.create("test");

            data = std::move(interpret);
        }

        ValueType targetV;
        interpreter.read(targetV, 0);
        Render::RenderTarget *target = ValueType_as<Render::RenderTarget *>(targetV);

        MeshRendererNodeInterpret *interpretData = static_cast<MeshRendererNodeInterpret *>(data.get());

        ValueType meshV;
        interpreter.read(meshV, 1);
        Render::GPUMeshLoader::ResourceType *mesh = ValueType_as<Render::GPUMeshLoader::ResourceType *>(meshV);

        if (mesh) {
            if (interpretData->mMesh.resource() != mesh)
                interpretData->mMesh = mesh;
            Material mat {
                0,
                0,
                { 1, 1, 1, 1 }
            };
            //TODO: Material;
            target->renderMesh(interpretData->mMesh, interpretData->mHandle, &mat);
        }
    }

    void MeshRendererNode::generate(NodeGraph::CodeGenerator &generator, IndexType<uint32_t> &flowInOut, std::unique_ptr<NodeGraph::CodeGeneratorData> &data) const
    {
        assert(flowInOut == 1);
        assert(!data);

        std::unique_ptr<ShaderCodeGeneratorData> _data = std::make_unique<ShaderCodeGeneratorData>();
        _data->mInstanceIndex = 0;
        data = std::move(_data);

        flowInOut = 1;
    }

    CodeGen::Statement MeshRendererNode::generateRead(NodeGraph::CodeGenerator &generator, uint32_t providerIndex, std::unique_ptr<NodeGraph::CodeGeneratorData> &data) const
    {
        return CodeGen::MemberAccess { std::string { sProviders[providerIndex].mName }, CodeGen::VariableRead { "IN" } };
    }

}
}

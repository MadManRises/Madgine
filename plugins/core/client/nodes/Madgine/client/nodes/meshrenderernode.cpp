#include "../../clientnodeslib.h"

#include "meshrenderernode.h"

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

    size_t MeshRendererNode::flowInCount(uint32_t group) const
    {
        return 1;
    }

    std::string_view MeshRendererNode::flowInName(uint32_t index, uint32_t group) const
    {
        return "execute";
    }

    size_t MeshRendererNode::flowOutBaseCount(uint32_t group) const
    {
        return 1;
    }

    std::string_view MeshRendererNode::flowOutName(uint32_t index, uint32_t group) const
    {
        return "return";
    }

    size_t MeshRendererNode::dataInBaseCount(uint32_t group) const
    {
        return 3;
    }

    std::string_view MeshRendererNode::dataInName(uint32_t index, uint32_t group) const
    {
        static constexpr std::array<std::string_view, 3> sInputNames {
            "target",
            "mesh",
            "pipeline"
        };
        return sInputNames[index];
    }

    ExtendedValueTypeDesc MeshRendererNode::dataInType(uint32_t index, uint32_t group, bool bidir) const
    {
        static constexpr std::array<ExtendedValueTypeDesc, 3> sInputTypes {
            toValueTypeDesc<Render::RenderTarget *>(),
            toValueTypeDesc<Render::GPUMeshLoader::Resource *>(),
            toValueTypeDesc<Render::PipelineInstance *>()
        };
        return sInputTypes[index];
    }

    struct MeshRendererNodeInterpret : NodeGraph::NodeInterpreterData {
        Render::GPUMeshLoader::Handle mMesh;
    };

    void MeshRendererNode::interpret(NodeGraph::NodeReceiver receiver, std::unique_ptr<NodeGraph::NodeInterpreterData> &data, uint32_t flowIn, uint32_t group) const
    {
        if (!data) {
            data = std::make_unique<MeshRendererNodeInterpret>();
        }

        MeshRendererNodeInterpret *interpretData = static_cast<MeshRendererNodeInterpret *>(data.get());

        ValueType in;
        receiver.read(in, 0);
        Render::RenderTarget *target = in.as<Render::RenderTarget *>();
        receiver.read(in, 1);
        Render::GPUMeshLoader::Resource *mesh = in.as<Render::GPUMeshLoader::Resource *>();
        receiver.read(in, 2);
        Render::PipelineInstance *pipeline = in.as<Render::PipelineInstance *>();

        if (mesh) {
            if (interpretData->mMesh.resource() != mesh)
                interpretData->mMesh = mesh;
            if (interpretData->mMesh.available()) {
                pipeline->renderMesh(target, interpretData->mMesh);
            }
        }

        receiver.set_value();
    }

    void MeshRendererNode::generate(NodeGraph::CodeGenerator &generator, std::unique_ptr<NodeGraph::CodeGeneratorData> &data, uint32_t flowIn, uint32_t group) const
    {
        assert(flowIn == 1);
        assert(!data);

        std::unique_ptr<ShaderCodeGeneratorData> _data = std::make_unique<ShaderCodeGeneratorData>();
        _data->mInstanceIndex = 0;
        data = std::move(_data);
    }

}
}

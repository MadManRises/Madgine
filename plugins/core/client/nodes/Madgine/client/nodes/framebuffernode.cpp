#include "../../clientnodeslib.h"

#include "framebuffernode.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "shadercodegenerator.h"

#include "Madgine/nodegraph/nodes/util/sendernode.h"

NODE(FrameBufferNode, Engine::Render::FrameBufferNode)

METATABLE_BEGIN_BASE(Engine::Render::FrameBufferNode, Engine::NodeGraph::NodeBase)
METATABLE_END(Engine::Render::FrameBufferNode)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Render::FrameBufferNode, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(Engine::Render::FrameBufferNode)

namespace Engine {
namespace Render {

    /*std::string_view Python3FunctionNode::name() const
        {
            return mName;
        }*/

    FrameBufferNode::FrameBufferNode(NodeGraph::NodeGraph &graph)
        : Node(graph)
    {
        setup();
    }

    FrameBufferNode::FrameBufferNode(const FrameBufferNode &other, NodeGraph::NodeGraph &graph)
        : Node(other, graph)
    {
    }

    size_t FrameBufferNode::flowInCount() const
    {
        return 1;
    }

    std::string_view FrameBufferNode::flowInName(uint32_t index) const
    {
        return "pixel";
    }

    uint32_t FrameBufferNode::flowInMask(uint32_t index, bool bidir) const
    {
        return NodeGraph::NodeExecutionMask::GPU;
    }

    size_t FrameBufferNode::dataInCount() const
    {
        return 1;
    }

    std::string_view FrameBufferNode::dataInName(uint32_t index) const
    {
        return "color";
    }

    ExtendedValueTypeDesc FrameBufferNode::dataInType(uint32_t index, bool bidir ) const
    {
        return toValueTypeDesc<Vector4>();
    }

    uint32_t FrameBufferNode::dataInMask(uint32_t index, bool bidir) const
    {
        return NodeGraph::NodeExecutionMask::GPU;
    }

    void FrameBufferNode::interpret(NodeGraph::NodeReceiver receiver, uint32_t flowIn, std::unique_ptr<NodeGraph::NodeInterpreterData> &data) const
    {
        throw 0;
    }

    void FrameBufferNode::generate(NodeGraph::CodeGenerator &_generator, IndexType<uint32_t> &flowInOut, std::unique_ptr<NodeGraph::CodeGeneratorData> &data) const
    {
        assert(flowInOut == 0);

        ShaderCodeGenerator &generator = dynamic_cast<ShaderCodeGenerator &>(_generator);

        CodeGen::ShaderFile &file = generator.mFile;

        file.statement(CodeGen::Return { {}, generator.read(0) });

        flowInOut.reset();
    }

}
}

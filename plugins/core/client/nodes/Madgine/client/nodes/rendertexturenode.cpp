#include "../../clientnodeslib.h"

#include "rendertexturenode.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "shadercodegenerator.h"

#include "Madgine/nodegraph/nodegraph.h"

#include "Madgine/nodegraph/nodes/util/sendernode.h"

NODE(RenderTextureNode, Engine::Render::RenderTextureNode)

METATABLE_BEGIN_BASE(Engine::Render::RenderTextureNode, Engine::NodeGraph::NodeBase)
METATABLE_END(Engine::Render::RenderTextureNode)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Render::RenderTextureNode, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(Engine::Render::RenderTextureNode)

namespace Engine {
namespace Render {

    RenderTextureNode::RenderTextureNode(NodeGraph::NodeGraph &graph)
        : Node(graph)
    {
        setup();
    }

    RenderTextureNode::RenderTextureNode(const RenderTextureNode &other, NodeGraph::NodeGraph &graph)
        : Node(other, graph)
    {
    }

    size_t RenderTextureNode::dataProviderBaseCount(uint32_t group) const
    {
        return 1;
    }

    std::string_view RenderTextureNode::dataProviderName(uint32_t index, uint32_t group) const
    {
        return "Target";
    }

    ExtendedValueTypeDesc RenderTextureNode::dataProviderType(uint32_t index, uint32_t group, bool bidir) const
    {
        return toValueTypeDesc<Render::RenderTarget *>();
    }

    bool RenderTextureNode::dataInVariadic(uint32_t group) const
    {
        return true;
    }

    std::string_view RenderTextureNode::dataInName(uint32_t index, uint32_t group) const
    {
        return "Pass";
    }

    ExtendedValueTypeDesc RenderTextureNode::dataInType(uint32_t index, uint32_t group, bool bidir) const
    {
        return toValueTypeDesc<Render::RenderPass*>();
    }

    void RenderTextureNode::interpret(NodeGraph::NodeReceiver receiver, std::unique_ptr<NodeGraph::NodeInterpreterData> &data, uint32_t flowIn, uint32_t group) const
    {
        throw 0;
    }

}
}

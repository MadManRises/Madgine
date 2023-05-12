#include "../../scenenodeslib.h"

#include "transformaccessornode.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Madgine/nodegraph/nodeinterpreter.h"

#include "Madgine/scene/entity/components/transform.h"

NODE(TransformAccessorNode, Engine::Scene::TransformAccessorNode)

METATABLE_BEGIN_BASE(Engine::Scene::TransformAccessorNode, Engine::NodeGraph::NodeBase)
METATABLE_END(Engine::Scene::TransformAccessorNode)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Scene::TransformAccessorNode, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(Engine::Scene::TransformAccessorNode)

namespace Engine {
namespace Scene {

    size_t TransformAccessorNode::dataProviderBaseCount(uint32_t group) const
    {
        return 1;
    }

    std::string_view TransformAccessorNode::dataProviderName(uint32_t index, uint32_t group) const
    {
        return "out";
    }

    ExtendedValueTypeDesc TransformAccessorNode::dataProviderType(uint32_t index, uint32_t group, bool bidir) const
    {
        return toValueTypeDesc<std::vector<Entity::Transform>>();
    }

    size_t TransformAccessorNode::dataReceiverBaseCount(uint32_t group) const
    {
        return 1;
    }

    std::string_view TransformAccessorNode::dataReceiverName(uint32_t index, uint32_t group) const
    {
        return "in";
    }

    ExtendedValueTypeDesc TransformAccessorNode::dataReceiverType(uint32_t index, uint32_t group, bool bidir) const
    {
        return toValueTypeDesc<std::vector<Entity::Transform>>();
    }

    struct TransformAccessorNodeInterpret : NodeGraph::NodeInterpreterData {
        TransformAccessorNodeInterpret()            
        {
        }
        
    };

    void TransformAccessorNode::interpretRead(NodeGraph::NodeInterpreter &interpreter, ValueType &retVal, std::unique_ptr<NodeGraph::NodeInterpreterData> &data, uint32_t providerIndex, uint32_t group) const
    {
        if (!data) {
            data = std::make_unique<TransformAccessorNodeInterpret>();
        }
        TransformAccessorNodeInterpret *variable = static_cast<TransformAccessorNodeInterpret *>(data.get());
    }

    void TransformAccessorNode::interpretWrite(NodeGraph::NodeInterpreter &interpreter, std::unique_ptr<NodeGraph::NodeInterpreterData> &data, const ValueType &v, uint32_t receiverIndex, uint32_t group) const
    {
        if (!data) {
            data = std::make_unique<TransformAccessorNodeInterpret>();
        }
        TransformAccessorNodeInterpret *variable = static_cast<TransformAccessorNodeInterpret *>(data.get());
    }

}
}

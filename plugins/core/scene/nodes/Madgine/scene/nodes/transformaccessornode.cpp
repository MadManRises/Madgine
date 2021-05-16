#include "../../scenenodeslib.h"

#include "transformaccessornode.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/keyvalueutil/valuetypeserialize.h"

#include "Madgine/nodegraph/nodeinterpreter.h"

#include "Madgine/scene/entity/components/transform.h"

NODE(TransformAccessorNode, Engine::Scene::TransformAccessorNode)

METATABLE_BEGIN_BASE(Engine::Scene::TransformAccessorNode, Engine::NodeGraph::NodeBase)
METATABLE_END(Engine::Scene::TransformAccessorNode)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Scene::TransformAccessorNode, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(Engine::Scene::TransformAccessorNode)

namespace Engine {
namespace Scene {

    TransformAccessorNode::TransformAccessorNode(NodeGraph::NodeGraph &graph)
        : Node(graph)
    {
        setup();
    }

    size_t TransformAccessorNode::dataProviderCount() const
    {
        return 1;
    }

    std::string_view TransformAccessorNode::dataProviderName(uint32_t index) const
    {
        return "out";
    }

    ExtendedValueTypeDesc TransformAccessorNode::dataProviderType(uint32_t index) const
    {
        return toValueTypeDesc<std::vector<Entity::Transform>>();
    }

    size_t TransformAccessorNode::dataReceiverCount() const
    {
        return 1;
    }

    std::string_view TransformAccessorNode::dataReceiverName(uint32_t index) const
    {
        return "in";
    }

    ExtendedValueTypeDesc TransformAccessorNode::dataReceiverType(uint32_t index) const
    {
        return toValueTypeDesc<std::vector<Entity::Transform>>();
    }

    struct TransformAccessorNodeInterpret : NodeGraph::NodeInterpreterData {
        TransformAccessorNodeInterpret()            
        {
        }
        
    };

    void TransformAccessorNode::interpretRead(NodeGraph::NodeInterpreter &interpreter, ValueType &retVal, uint32_t providerIndex, std::unique_ptr<NodeGraph::NodeInterpreterData> &data) const
    {
        if (!data) {
            data = std::make_unique<TransformAccessorNodeInterpret>();
        }
        TransformAccessorNodeInterpret *variable = static_cast<TransformAccessorNodeInterpret *>(data.get());
    }

    void TransformAccessorNode::interpretWrite(NodeGraph::NodeInterpreter &interpreter, uint32_t receiverIndex, std::unique_ptr<NodeGraph::NodeInterpreterData> &data, const ValueType &v) const
    {
        if (!data) {
            data = std::make_unique<TransformAccessorNodeInterpret>();
        }
        TransformAccessorNodeInterpret *variable = static_cast<TransformAccessorNodeInterpret *>(data.get());
    }

}
}

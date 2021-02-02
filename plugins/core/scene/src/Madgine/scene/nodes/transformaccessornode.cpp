#include "../../scenelib.h"

#include "transformaccessornode.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Modules/keyvalueutil/valuetypeserialize.h"

#include "Madgine/nodegraph/nodeinterpreter.h"

#include "../entity/components/transform.h"

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

    ValueType TransformAccessorNode::interpretRead(NodeGraph::NodeInterpreter &interpreter, uint32_t providerIndex, std::unique_ptr<NodeGraph::NodeInterpreterData> &data) const
    {
        if (!data) {
            data = std::make_unique<TransformAccessorNodeInterpret>();
        }
        TransformAccessorNodeInterpret *variable = static_cast<TransformAccessorNodeInterpret *>(data.get());
        return {};
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

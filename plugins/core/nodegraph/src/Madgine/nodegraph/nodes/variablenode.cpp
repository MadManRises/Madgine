#include "../../nodegraphlib.h"

#include "variablenode.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"
#include "Modules/serialize/serializetable_impl.h"

#include "Modules/keyvalueutil/valuetypeserialize.h"

#include "../nodeinterpreter.h"

NODE(VariableNode, Engine::NodeGraph::VariableNode)

METATABLE_BEGIN_BASE(Engine::NodeGraph::VariableNode, Engine::NodeGraph::NodeBase)
MEMBER(mDefaultValue)
METATABLE_END(Engine::NodeGraph::VariableNode)

SERIALIZETABLE_INHERIT_BEGIN(Engine::NodeGraph::VariableNode, Engine::NodeGraph::NodeBase)
FIELD(mDefaultValue)
SERIALIZETABLE_END(Engine::NodeGraph::VariableNode)

namespace Engine {
namespace NodeGraph {

    VariableNode::VariableNode(NodeGraph &graph)
        : Node(graph)
    {
        setup();
    }

    size_t VariableNode::dataProviderCount() const
    {
        return 1;
    }

    std::string_view VariableNode::dataProviderName(uint32_t index) const
    {
        return "out";
    }

    ExtendedValueTypeDesc VariableNode::dataProviderType(uint32_t index) const
    {
        return mDefaultValue.type();
    }

    size_t VariableNode::dataReceiverCount() const
    {
        return 1;
    }

    std::string_view VariableNode::dataReceiverName(uint32_t index) const
    {
        return "in";
    }

    ExtendedValueTypeDesc VariableNode::dataReceiverType(uint32_t index) const
    {
        return mDefaultValue.type();
    }

    struct VariableNodeInterpret : NodeInterpreterData {
        VariableNodeInterpret(ValueType data)
            : mData(std::move(data))
        {
        }

        ValueType mData;
    };

    ValueType VariableNode::interpretRead(NodeInterpreter &interpreter, uint32_t providerIndex, std::unique_ptr<NodeInterpreterData> &data) const
    {
        if (!data) {
            data = std::make_unique<VariableNodeInterpret>(mDefaultValue);
        }
        VariableNodeInterpret *variable = static_cast<VariableNodeInterpret *>(data.get());
        return variable->mData;
    }

    void VariableNode::interpretWrite(NodeInterpreter &interpreter, uint32_t receiverIndex, std::unique_ptr<NodeInterpreterData> &data, const ValueType &v) const
    {
        if (!data) {
            data = std::make_unique<VariableNodeInterpret>(mDefaultValue);
        }
        VariableNodeInterpret *variable = static_cast<VariableNodeInterpret *>(data.get());
        variable->mData = v;
    }

}
}

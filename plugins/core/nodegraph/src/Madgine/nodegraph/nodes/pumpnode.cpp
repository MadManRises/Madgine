#include "../../nodegraphlib.h"

#include "pumpnode.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "../nodeinterpreter.h"

#include "../nodegraph.h"

NODE(PumpNode, Engine::NodeGraph::PumpNode)

METATABLE_BEGIN_BASE(Engine::NodeGraph::PumpNode, Engine::NodeGraph::NodeBase)
METATABLE_END(Engine::NodeGraph::PumpNode)

SERIALIZETABLE_INHERIT_BEGIN(Engine::NodeGraph::PumpNode, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(Engine::NodeGraph::PumpNode)

namespace Engine {
namespace NodeGraph {

    PumpNode::PumpNode(NodeGraph &graph)
        : Node(graph)
    {
        setup();
    }

    PumpNode::PumpNode(const PumpNode &other, NodeGraph &graph)
        : Node(other, graph)
    {
    }

    size_t PumpNode::flowInCount() const
    {
        return 1;
    }

    std::string_view PumpNode::flowInName(uint32_t index) const
    {
        return "pump";
    }

    size_t PumpNode::flowOutCount() const
    {
        return 1;
    }

    std::string_view PumpNode::flowOutName(uint32_t index) const
    {
        return "return";
    }

    size_t PumpNode::dataInCount() const
    {
        return 1;
    }

    std::string_view PumpNode::dataInName(uint32_t index) const
    {
        return "read";
    }

    ExtendedValueTypeDesc PumpNode::dataInType(uint32_t index, bool bidir) const
    {
        assert(index == 0);
        ExtendedValueTypeDesc desc { ExtendedValueTypeEnum::GenericType };
        if (mDataOutPins[0].mTarget)
            desc = mGraph.dataReceiverType(mDataOutPins[0].mTarget, false);
        if (bidir && desc.mType == ExtendedValueTypeEnum::GenericType)
            return dataOutType(index, false);
        return desc;
    }

    size_t PumpNode::dataOutCount() const
    {
        return 1;
    }

    std::string_view PumpNode::dataOutName(uint32_t index) const
    {
        return "write";
    }

    ExtendedValueTypeDesc PumpNode::dataOutType(uint32_t index, bool bidir) const
    {
        assert(index == 0);
        ExtendedValueTypeDesc desc { ExtendedValueTypeEnum::GenericType };
        if (mDataInPins[0].mSource)
            desc = mGraph.dataProviderType(mDataInPins[0].mSource, false);
        if (bidir && desc.mType == ExtendedValueTypeEnum::GenericType)
            return dataInType(index, false);
        return desc;
    }

    void PumpNode::interpret(NodeInterpreter &interpreter, IndexType<uint32_t> &flowInOut, std::unique_ptr<NodeInterpreterData> &data) const
    {
        ValueType v;
        interpreter.read(v, 0);
        interpreter.write(0, v);
    }

}
}

#include "../../nodegraphlib.h"

#include "testnode.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "../nodeinterpreter.h"

#include "Meta/keyvalue/valuetype.h"

#include "util/sendernode.h"

NODE(TestNode, Engine::NodeGraph::TestNode)

METATABLE_BEGIN_BASE(Engine::NodeGraph::TestNode, Engine::NodeGraph::NodeBase)
MEMBER(mDummy)
METATABLE_END(Engine::NodeGraph::TestNode)

SERIALIZETABLE_INHERIT_BEGIN(Engine::NodeGraph::TestNode, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(Engine::NodeGraph::TestNode)

namespace Engine {
namespace NodeGraph {

    TestNode::TestNode(NodeGraph &graph)
        : Node(graph)
    {
        setup();
    }

    TestNode::TestNode(const TestNode &other, NodeGraph &graph)
        : Node(other, graph)
    {
    }

    size_t TestNode::flowInCount() const
    {
        return 1;
    }

    size_t TestNode::flowOutCount() const
    {
        return 1;
    }

    size_t TestNode::dataInCount() const
    {
        return 2;
    }

    std::string_view TestNode::dataInName(uint32_t index) const
    {
        return "data_in";
    }

    ExtendedValueTypeDesc TestNode::dataInType(uint32_t index, bool bidir) const
    {
        return { ValueTypeIndex { index == 0 ? ValueTypeEnum::IntValue : ValueTypeEnum::FloatValue } };
    }

    size_t TestNode::dataOutCount() const
    {
        return 1;
    }

    std::string_view TestNode::dataOutName(uint32_t index) const
    {
        return "data_out";
    }

    ExtendedValueTypeDesc TestNode::dataOutType(uint32_t index, bool bidir) const
    {
        return { ValueTypeIndex { ValueTypeEnum::FloatValue } };
    }

    void TestNode::interpret(NodeReceiver receiver, uint32_t flowIn, std::unique_ptr<NodeInterpreterData> &data) const
    {
        ValueType v;
        receiver.read(v, 1);
        ValueType v2;
        receiver.read(v2, 0);
        float f = v.as<float>();
        int i = v2.as<int>();
        LOG(i << " * " << f << " -> " << f * i);
        receiver.write(0, ValueType { i * f });
        receiver.set_value();
    }

}
}

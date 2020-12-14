#include "../../nodegraphlib.h"

#include "testnode.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"
//#include "Modules/serialize/serializetable_impl.h"

NODE(TestNode, Engine::NodeGraph::TestNode)

METATABLE_BEGIN(Engine::NodeGraph::TestNode)
METATABLE_END(Engine::NodeGraph::TestNode)
/*
SERIALIZETABLE_BEGIN(Engine::Scene::Entity::Skeleton)
ENCAPSULATED_FIELD(Skeleton, getName, setName)
SERIALIZETABLE_END(Engine::Scene::Entity::Skeleton)
*/


namespace Engine {
namespace NodeGraph {

    TestNode::TestNode()
    {
        mFlowOutPins.emplace_back();
        mDataOutPins.emplace_back(DataOutPinPrototype { ValueTypeIndex { ValueTypeEnum::IntValue } });
        mDataInPins.emplace_back();
        mDataInPins.emplace_back();
    }

    size_t TestNode::flowInCount() const
    {
        return 1;
    }

    std::string_view TestNode::flowInName(uint32_t index) const
    {
        return "execute";
    }

    std::string_view TestNode::flowOutName(uint32_t index) const
    {
        return "1";
    }

    std::string_view TestNode::dataInName(uint32_t index) const
    {
        return "data_in";
    }

    std::string_view TestNode::dataOutName(uint32_t index) const
    {
        return "data_out";
    }

    void TestNode::execute(NodeInstance *instance, uint32_t flowIndex, ArgumentList *out) const
    {
        throw 0;
    }

    ExtendedValueTypeDesc TestNode::dataInExpectedType(uint32_t index) const
    {
        return { ValueTypeIndex { index == 0 ? ValueTypeEnum::IntValue : ValueTypeEnum::FloatValue } };
    }

}
}

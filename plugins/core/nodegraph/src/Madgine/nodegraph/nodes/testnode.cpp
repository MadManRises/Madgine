#include "../../nodegraphlib.h"

#include "testnode.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"
//#include "Modules/serialize/serializetable_impl.h"

NODE(TestNode, Engine::NodeGraph::TestNode)

METATABLE_BEGIN(Engine::NodeGraph::TestNode)
METATABLE_END(Engine::NodeGraph::TestNode)
/*
SERIALIZETABLE_BEGIN(Engine::Scene::Entity::Skeleton)
ENCAPSULATED_FIELD(Skeleton, getName, setName)
SERIALIZETABLE_END(Engine::Scene::Entity::Skeleton)
*/
RegisterType(Engine::NodeGraph::TestNode);

namespace Engine {
namespace NodeGraph {

    TestNode::TestNode()
    {
        mFlowOutPins.emplace_back();
        mDataOutPins.emplace_back(DataOutPinPrototype { ValueType::Type::IntValue });
        mDataInPins.emplace_back();
        mDataInPins.emplace_back();
    }

    size_t TestNode::flowInCount() const
    {
        return 1;
    }

    std::string_view TestNode::flowInName(size_t index) const
    {
        return "execute";
    }

    std::string_view TestNode::flowOutName(size_t index) const
    {
        return "1";
    }

    std::string_view TestNode::dataInName(size_t index) const
    {
        return "data_in";
    }

    std::string_view TestNode::dataOutName(size_t index) const
    {
        return "data_out";
    }

    void TestNode::execute(NodeInstance *instance, size_t flowIndex, ArgumentList *out) const
    {
        throw 0;
    }

    ValueType::ExtendedTypeDesc TestNode::dataInExpectedType(size_t index) const
    {
        return { index == 0 ? ValueType::Type::IntValue : ValueType::Type::FloatValue };
    }

}
}
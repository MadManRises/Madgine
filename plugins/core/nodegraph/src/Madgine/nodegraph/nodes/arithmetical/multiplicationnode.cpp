#include "../../../nodegraphlib.h"

#include "multiplicationnode.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "../../codegenerator.h"

NODE(MultiplicationNode, Engine::NodeGraph::MultiplicationNode)

METATABLE_BEGIN_BASE(Engine::NodeGraph::MultiplicationNode, Engine::NodeGraph::NodeBase)
METATABLE_END(Engine::NodeGraph::MultiplicationNode)

SERIALIZETABLE_INHERIT_BEGIN(Engine::NodeGraph::MultiplicationNode, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(Engine::NodeGraph::MultiplicationNode)

namespace Engine {
namespace NodeGraph {

    MultiplicationNode::MultiplicationNode(NodeGraph &graph)
        : Node(graph)
    {
        setup();
    }

    MultiplicationNode::MultiplicationNode(const MultiplicationNode &other, NodeGraph &graph)
        : Node(other, graph)
    {
    }

    size_t MultiplicationNode::dataInCount() const
    {
        return 2;
    }

    ExtendedValueTypeDesc MultiplicationNode::dataInType(uint32_t index, bool bidir) const
    {
        return { ExtendedValueTypeEnum::GenericType };
    }

    std::string_view MultiplicationNode::dataInName(uint32_t index) const
    {
        return "factor";
    }

    size_t MultiplicationNode::dataProviderCount() const
    {
        return 1;
    }

    ExtendedValueTypeDesc MultiplicationNode::dataProviderType(uint32_t index, bool bidir) const
    {
        return { ExtendedValueTypeEnum::GenericType };
    }

    std::string_view MultiplicationNode::dataProviderName(uint32_t index) const
    {
        return "result";
    }

    CodeGen::Statement MultiplicationNode::generateRead(CodeGenerator &generator, uint32_t providerIndex, std::unique_ptr<CodeGeneratorData> &data) const
    {
        return CodeGen::ArithOperation {
            CodeGen::ArithOperation::MUL, { generator.read(0), generator.read(1) }
        };
    }

}
}
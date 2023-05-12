#include "../../../nodegraphlib.h"

#include "additionnode.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "../../codegenerator.h"

NODE(AdditionNode, Engine::NodeGraph::AdditionNode)

METATABLE_BEGIN_BASE(Engine::NodeGraph::AdditionNode, Engine::NodeGraph::NodeBase)
METATABLE_END(Engine::NodeGraph::AdditionNode)

SERIALIZETABLE_INHERIT_BEGIN(Engine::NodeGraph::AdditionNode, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(Engine::NodeGraph::AdditionNode)

namespace Engine {
namespace NodeGraph {

    AdditionNode::AdditionNode(NodeGraph &graph)
        : Node(graph)
    {
        setup();
    }

    AdditionNode::AdditionNode(const AdditionNode &other, NodeGraph &graph)
        : Node(other, graph)
    {
    }

    size_t AdditionNode::dataInBaseCount(uint32_t group) const
    {
        return 2;
    }

    ExtendedValueTypeDesc AdditionNode::dataInType(uint32_t index, uint32_t group, bool bidir) const
    {
        ExtendedValueTypeDesc desc { ExtendedValueTypeEnum::GenericType };
        if (!mDataProviderPins[0][0].mTargets.empty()) {
            desc = mGraph.dataInType(mDataProviderPins[0][0].mTargets.front(), false);
            if (desc.mType != ExtendedValueTypeEnum::GenericType)
                return desc;
        }
        for (uint32_t i = 0; i < 2; ++i) {
            if (bidir || i != index) {
                if (mDataInPins[0][i].mSource) {
                    desc = mGraph.dataProviderType(mDataInPins[0][i].mSource, false);
                    if (desc.mType != ExtendedValueTypeEnum::GenericType)
                        return desc;
                }
            }
        }
        return desc;
    }

    std::string_view AdditionNode::dataInName(uint32_t index, uint32_t group) const
    {
        return "summand";
    }

    size_t AdditionNode::dataProviderBaseCount(uint32_t group) const
    {
        return 1;
    }

    ExtendedValueTypeDesc AdditionNode::dataProviderType(uint32_t index, uint32_t group, bool bidir) const
    {
        ExtendedValueTypeDesc desc { ExtendedValueTypeEnum::GenericType };
        for (uint32_t i = 0; i < 2; ++i) {
            if (mDataInPins[0][i].mSource) {
                desc = mGraph.dataProviderType(mDataInPins[0][i].mSource, false);
                if (desc.mType != ExtendedValueTypeEnum::GenericType)
                    return desc;
            }
        }
        if (bidir && desc.mType == ExtendedValueTypeEnum::GenericType) {
            if (!mDataProviderPins[0][0].mTargets.empty()) {
                desc = mGraph.dataInType(mDataProviderPins[0][0].mTargets.front(), false);
                if (desc.mType != ExtendedValueTypeEnum::GenericType)
                    return desc;
            }
        }
        return desc;
    }

    std::string_view AdditionNode::dataProviderName(uint32_t index, uint32_t group) const
    {
        return "result";
    }

    CodeGen::Statement AdditionNode::generateRead(CodeGenerator &generator, uint32_t providerIndex, std::unique_ptr<CodeGeneratorData> &data) const
    {
        return CodeGen::ArithOperation {
            CodeGen::ArithOperation::ADD, { generator.read(0), generator.read(1) }
        };
    }

}
}
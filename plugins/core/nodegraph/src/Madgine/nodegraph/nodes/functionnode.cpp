#include "../../nodegraphlib.h"

#include "functionnode.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/keyvalueutil/valuetypeserialize.h"

#include "../nodeinterpreter.h"

#include "../codegenerator.h"

#include "Meta/keyvalue/functiontable.h"

#include "../nodeexecution.h"

NODE(FunctionNode, Engine::NodeGraph::FunctionNode)

METATABLE_BEGIN_BASE(Engine::NodeGraph::FunctionNode, Engine::NodeGraph::NodeBase)
PROPERTY(Function, getFunction, setFunction)
METATABLE_END(Engine::NodeGraph::FunctionNode)

SERIALIZETABLE_INHERIT_BEGIN(Engine::NodeGraph::FunctionNode, Engine::NodeGraph::NodeBase)
ENCAPSULATED_FIELD(Function, getFunctionName, setFunctionName)
SERIALIZETABLE_END(Engine::NodeGraph::FunctionNode)

namespace Engine {
namespace NodeGraph {

    FunctionNode::FunctionNode(NodeGraph &graph)
        : Node(graph)
    {
        setup();
    }

    FunctionNode::FunctionNode(const FunctionNode &other, NodeGraph &graph)
        : Node(other, graph)
        , mFunction(other.mFunction)
    {
    }

    size_t FunctionNode::dataInBaseCount(uint32_t group) const
    {
        return mFunction ? mFunction->mArgumentsCount : 0;
    }

    std::string_view FunctionNode::dataInName(uint32_t index, uint32_t group) const
    {
        return mFunction->mArguments[index].mName;
    }

    ExtendedValueTypeDesc FunctionNode::dataInType(uint32_t index, uint32_t group, bool bidir) const
    {
        return mFunction->mArguments[index].mType;
    }

    size_t FunctionNode::dataProviderBaseCount(uint32_t group) const
    {
        return mFunction ? 1 : 0;
    }

    ExtendedValueTypeDesc FunctionNode::dataProviderType(uint32_t index, uint32_t group, bool bidir) const
    {
        return mFunction->mReturnType;
    }

    void FunctionNode::interpretRead(NodeInterpreterState &interpreter, ValueType &retVal, std::unique_ptr<NodeInterpreterData> &data, uint32_t providerIndex, uint32_t group) const
    {
        ArgumentList arguments { dataInCount() };
        for (size_t i = 0; i < dataInCount(); ++i) {
            NodeInterpretHandle<NodeBase> { interpreter, *this }.read(arguments[i], i);
        }
        mFunction->mFunctionPtr(mFunction, retVal, arguments);
    }

    CodeGen::Statement FunctionNode::generateRead(CodeGenerator &generator, std::unique_ptr<CodeGeneratorData> &data, uint32_t providerIndex, uint32_t group) const
    {
        throw 0;
    }

    void FunctionNode::setFunction(FunctionTable *function)
    {
        mFunction = function;
        setup();
    }

    FunctionTable *FunctionNode::getFunction() const
    {
        return mFunction;
    }

    std::string_view FunctionNode::getFunctionName() const
    {
        return mFunction ? mFunction->mName : "";
    }

    void FunctionNode::setFunctionName(std::string_view name)
    {
        const FunctionTable *table = sFunctionList();
        while (table) {
            if (table->mName == name) {
                setFunction(const_cast<FunctionTable *>(table));
                return;
            }
            table = table->mNext;
        }
    }

}
}

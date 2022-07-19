#include "../../nodegraphlib.h"

#include "functionnode.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/keyvalueutil/valuetypeserialize.h"

#include "../nodeinterpreter.h"

#include "../codegenerator.h"

#include "Meta/keyvalue/functiontable.h"

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
        : Node(other, graph),
        mFunction(other.mFunction)
    {
    }

    size_t FunctionNode::dataInCount() const
    {
        return mFunction ? mFunction->mArgumentsCount : 0;
    }

    std::string_view FunctionNode::dataInName(uint32_t index) const
    {
        return mFunction->mArguments[index].mName;
    }

    ExtendedValueTypeDesc FunctionNode::dataInType(uint32_t index, bool bidir ) const
    {
        return mFunction->mArguments[index].mType;
    }

    size_t FunctionNode::dataProviderCount() const
    {
        return mFunction ? 1 : 0;
    }

    std::string_view FunctionNode::dataProviderName(uint32_t index) const
    {
        return "call";
    }

    ExtendedValueTypeDesc FunctionNode::dataProviderType(uint32_t index, bool bidir ) const
    {
        return mFunction->mReturnType;
    }

    void FunctionNode::interpretRead(NodeInterpreter &interpreter, ValueType &retVal, uint32_t providerIndex, std::unique_ptr<NodeInterpreterData> &data) const
    {
        ArgumentList arguments { dataInCount() };
        for (size_t i = 0; i < dataInCount(); ++i) {
            interpreter.read(arguments[i], i);
        }
        mFunction->mFunctionPtr(mFunction, retVal, arguments);        
    }

    CodeGen::Statement FunctionNode::generateRead(CodeGenerator &generator, uint32_t providerIndex, std::unique_ptr<CodeGeneratorData> &data) const
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

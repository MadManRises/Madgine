#include "../nodegraphlib.h"

#include "nodeexecution.h"

#include "nodebase.h"
#include "nodeinterpreter.h"

#include "codegenerator.h"

namespace Engine {
namespace NodeGraph {

    void NodeInterpretHandleBase::read(const NodeBase &node, ValueType &retVal, uint32_t dataInIndex, uint32_t group)
    {
        Pin pin = node.dataInSource(dataInIndex, group);
        mInterpreter.read(retVal, pin);
    }

    void NodeInterpretHandleBase::write(const NodeBase &node, const ValueType &v, uint32_t dataOutIndex, uint32_t group)
    {
        Pin pin = node.dataOutTarget(dataOutIndex, group);
        mInterpreter.write(pin, v);
    }

    ValueType NodeInterpretHandleBase::resolveVar(std::string_view name)
    {
        ValueType v;
        bool result = mInterpreter.resolveVar(v, name);
        assert(result);
        return v;
    }

    void continueExecution(NodeInterpreterState &interpreter, const NodeBase &node, Execution::VirtualReceiverBase<InterpretResult> &receiver)
    {
        Pin pin = node.flowOutTarget(0);
        interpreter.branch(receiver, pin);
    }

    CodeGen::Statement NodeCodegenHandle::read(uint32_t dataInIndex, uint32_t group)
    {
        return mGenerator.read(mNode->dataInSource(dataInIndex, group));
    }

}
}
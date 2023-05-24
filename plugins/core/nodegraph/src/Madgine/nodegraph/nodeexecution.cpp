#include "../nodegraphlib.h"

#include "nodeexecution.h"

#include "nodebase.h"
#include "nodeinterpreter.h"

namespace Engine {
namespace NodeGraph {

    void NodeInterpretHandle::read(ValueType &retVal, uint32_t dataInIndex, uint32_t group)
    {
        Pin pin = mNode->dataInSource(dataInIndex, group);
        mInterpreter.read(retVal, pin);
    }

    void NodeInterpretHandle::write(const ValueType &v, uint32_t dataOutIndex, uint32_t group)
    {
        Pin pin = mNode->dataOutTarget(dataOutIndex, group);
        mInterpreter.write(pin, v);
    }

    ValueType NodeInterpretHandle::resolveVar(std::string_view name)
    {
        ValueType v;
        bool result = mInterpreter.resolveVar(v, name);
        assert(result);
        return v;
    }

    void NodeReceiver::set_value()
    {
        Pin pin = mNode->flowOutTarget(0);
        mInterpreter.branch(mReceiver, pin);
    }

    void NodeReceiver::set_done()
    {
        mReceiver.set_done();
    }

    void NodeReceiver::set_error(GenericResult result)
    {
        mReceiver.set_error(result);
    }

}
}
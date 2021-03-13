#include "../nodegraphlib.h"

#include "nodebase.h"

#include "Meta/serialize/streams/serializestream.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Meta/keyvalueutil/valuetypeserialize.h"

METATABLE_BEGIN(Engine::NodeGraph::NodeBase)
METATABLE_END(Engine::NodeGraph::NodeBase)

SERIALIZETABLE_BEGIN(Engine::NodeGraph::NodeBase)
FIELD(mFlowOutPins)
FIELD(mDataInPins)
FIELD(mDataOutPins)
SERIALIZETABLE_END(Engine::NodeGraph::NodeBase)

namespace Engine {
namespace NodeGraph {

    TargetPin::operator bool() const
    {
        return bool(mIndex);
    }

    NodeBase::NodeBase(NodeGraph &graph)
        : mGraph(graph)
    {
    }

    uint32_t NodeBase::flowInId(uint32_t index)
    {
        return 1 + index;
    }

    TargetPin NodeBase::flowOutTarget(uint32_t index) const
    {
        return mFlowOutPins[index].mTarget;
    }

    uint32_t NodeBase::flowOutId(uint32_t index)
    {
        return 1001 + index;
    }

    TargetPin NodeBase::dataInSource(uint32_t index) const
    {
        return mDataInPins[index].mSource;
    }

    uint32_t NodeBase::dataInId(uint32_t index)
    {
        return 2001 + index;
    }

    TargetPin NodeBase::dataOutTarget(uint32_t index) const
    {
        return mDataOutPins[index].mTarget;
    }

    uint32_t NodeBase::dataOutId(uint32_t index)
    {
        return 3001 + index;
    }

    uint32_t NodeBase::dataReceiverId(uint32_t index)
    {
        return 4001 + index;
    }

    uint32_t NodeBase::dataProviderId(uint32_t index)
    {
        return 5001 + index;
    }

    PinDesc NodeBase::pinFromId(uint32_t id)
    {
        assert(id < 6000);
        PinDesc desc;
        desc.mDir = ((id % 2000) < 1000 ? PinDir::In : PinDir::Out);
        desc.mType = (id < 4000 ? (id < 2000 ? PinType::Flow : PinType::Data) : PinType::DataInstance);
        desc.mIndex = id % 1000 - 1;
        return desc;
    }

    void NodeBase::connectFlow(uint32_t index, uint32_t target, uint32_t targetIndex)
    {
        mFlowOutPins[index].mTarget = { target, targetIndex };
    }    

    void NodeBase::connectDataIn(uint32_t index, uint32_t source, uint32_t sourceIndex)
    {
        mDataInPins[index].mSource = { source, sourceIndex };
    }
    
    void NodeBase::connectDataOut(uint32_t index, uint32_t target, uint32_t targetIndex)
    {
        mDataOutPins[index].mTarget = { target, targetIndex };
    }

    void NodeBase::disconnectFlow(uint32_t index)
    {
        mFlowOutPins[index].mTarget = {};
    }

    void NodeBase::disconnectDataIn(uint32_t index)
    {
        mDataInPins[index].mSource = {};
    }

    void NodeBase::disconnectDataOut(uint32_t index)
    {
        mDataOutPins[index].mTarget = {};
    }

    uint32_t NodeBase::interpret(NodeInterpreter &interpreter, uint32_t flowIn, std::unique_ptr<NodeInterpreterData> &data) const
    {
        throw 0;
    }

    ValueType NodeBase::interpretRead(NodeInterpreter &interpreter, uint32_t providerIndex, std::unique_ptr<NodeInterpreterData> &data) const
    {
        throw 0;
    }

    void NodeBase::interpretWrite(NodeInterpreter &interpreter, uint32_t receiverIndex, std::unique_ptr<NodeInterpreterData> &data, const ValueType &v) const
    {
        throw 0;
    }

    void NodeBase::setup()
    {
        mFlowOutPins.clear();
        mFlowOutPins.resize(flowOutCount());
        mDataInPins.clear();
        mDataInPins.resize(dataInCount());
        mDataOutPins.clear();
        mDataOutPins.resize(dataOutCount());
    }

}
}

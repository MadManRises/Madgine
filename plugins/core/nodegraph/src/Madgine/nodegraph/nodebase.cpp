#include "../nodegraphlib.h"

#include "nodebase.h"

#include "Meta/serialize/streams/serializestream.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Meta/keyvalue/valuetype.h"
#include "Meta/keyvalueutil/valuetypeserialize.h"

#include "nodegraph.h"

METATABLE_BEGIN(Engine::NodeGraph::NodeBase)
METATABLE_END(Engine::NodeGraph::NodeBase)

SERIALIZETABLE_BEGIN(Engine::NodeGraph::NodeBase)
FIELD(mFlowOutPins)
FIELD(mDataInPins)
FIELD(mDataOutPins)
SERIALIZETABLE_END(Engine::NodeGraph::NodeBase)

namespace Engine {
namespace NodeGraph {

    Pin::operator bool() const
    {
        return bool(mIndex);
    }

    NodeBase::NodeBase(NodeGraph &graph)
        : mGraph(graph)
    {
    }

    NodeBase::NodeBase(const NodeBase &other, NodeGraph &graph)
        : mGraph(graph)
        , mFlowOutPins(other.mFlowOutPins)
        , mDataInPins(other.mDataInPins)
        , mDataOutPins(other.mDataOutPins)
        , mDataProviderPins(other.mDataProviderPins)
        , mDataReceiverPins(other.mDataReceiverPins)
        , mFlowInPins(other.mFlowInPins)
    {
    }

    uint32_t NodeBase::flowInId(uint32_t index)
    {
        return 1 + index;
    }

    Pin NodeBase::flowOutTarget(uint32_t index) const
    {
        return mFlowOutPins[index].mTarget;
    }

    uint32_t NodeBase::flowOutId(uint32_t index)
    {
        return 1001 + index;
    }

    ValueType NodeBase::dataInDefault(uint32_t index) const
    {
        return {};
    }

    Pin NodeBase::dataInSource(uint32_t index) const
    {
        if (index == mDataInPins.size() && dataInVariadic()) {
            return {};
        }
        return mDataInPins[index].mSource;
    }

    uint32_t NodeBase::dataInId(uint32_t index)
    {
        return 2001 + index;
    }

    Pin NodeBase::dataOutTarget(uint32_t index) const
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
        uint32_t smallId = id % 6000;
        PinDesc desc;
        desc.mDir = ((smallId % 2000) < 1000 ? PinDir::In : PinDir::Out);
        desc.mType = (smallId < 4000 ? (smallId < 2000 ? PinType::Flow : PinType::Data) : PinType::DataInstance);
        desc.mPin = { id / 6000, id % 1000 - 1 };
        return desc;
    }

    void NodeBase::onFlowOutUpdate(uint32_t index, Pin target, EdgeEvent event)
    {
    }

    void NodeBase::onDataInUpdate(uint32_t index, Pin source, EdgeEvent event)
    {
    }

    void NodeBase::onDataOutUpdate(uint32_t index, Pin target, EdgeEvent event)
    {
    }

    void NodeBase::onFlowInUpdate(uint32_t index, Pin source, EdgeEvent event)
    {
    }

    void NodeBase::onDataProviderUpdate(uint32_t index, Pin target, EdgeEvent event)
    {
    }

    void NodeBase::onDataReceiverUpdate(uint32_t index, Pin source, EdgeEvent event)
    {
    }

    void NodeBase::onDataProviderRemove(Pin pin)
    {
        for (uint32_t i = 0; i < dataInCount(); ++i) {
            Pin &target = mDataInPins[i].mSource;
            if (target.mNode == pin.mNode) {
                if (target.mIndex == pin.mIndex) {
                    mGraph.disconnectDataIn({ mGraph.nodeIndex(this), i });
                } else if (target && target.mIndex > pin.mIndex) {
                    --target.mIndex;
                }
            }
        }
    }

    void NodeBase::onNodeRemove(uint32_t oldIndex, uint32_t newIndex)
    {
        for (FlowOutPinPrototype& pin : mFlowOutPins) {
            if (pin.mTarget.mNode == newIndex)
                pin.mTarget = {};
            else if (pin.mTarget.mNode == oldIndex)
                pin.mTarget.mNode = newIndex;
        }
        for (DataInPinPrototype &pin : mDataInPins) {
            if (pin.mSource.mNode == newIndex)
                pin.mSource = {};
            else if (pin.mSource.mNode == oldIndex)
                pin.mSource.mNode = newIndex;
        }
        for (DataOutPinPrototype &pin : mDataOutPins) {
            if (pin.mTarget.mNode == newIndex)
                pin.mTarget = {};
            else if (pin.mTarget.mNode == oldIndex)
                pin.mTarget.mNode = newIndex;
        }

        for (FlowInPinPrototype& pin : mFlowInPins) {
            for (Pin &source : pin.mSources) {
                if (source.mNode == newIndex)
                    source = {};
                else if (source.mNode == oldIndex)
                    source.mNode = newIndex;
            }
        }
        for (DataReceiverPinPrototype &pin : mDataReceiverPins) {
            for (Pin &source : pin.mSources) {
                if (source.mNode == newIndex)
                    source = {};
                else if (source.mNode == oldIndex)
                    source.mNode = newIndex;
            }
        }
        for (DataProviderPinPrototype &pin : mDataProviderPins) {
            for (Pin &target : pin.mTargets) {
                if (target.mNode == newIndex)
                    target = {};
                else if (target.mNode == oldIndex)
                    target.mNode = newIndex;
            }
        }
    }

    void NodeBase::interpret(NodeInterpreter &interpreter, IndexType<uint32_t> &flowInOut, std::unique_ptr<NodeInterpreterData> &data) const
    {
        throw 0;
    }

    void NodeBase::interpretRead(NodeInterpreter &interpreter, ValueType &retVal, uint32_t providerIndex, std::unique_ptr<NodeInterpreterData> &data) const
    {
        throw 0;
    }

    void NodeBase::interpretWrite(NodeInterpreter &interpreter, uint32_t receiverIndex, std::unique_ptr<NodeInterpreterData> &data, const ValueType &v) const
    {
        throw 0;
    }

    void NodeBase::generate(CodeGenerator &generator, IndexType<uint32_t> &flowInOut, std::unique_ptr<CodeGeneratorData> &data) const
    {
        throw 0;
    }

    CodeGen::Statement NodeBase::generateRead(CodeGenerator &generator, uint32_t providerIndex, std::unique_ptr<CodeGeneratorData> &data) const
    {
        throw 0;
    }

    CodeGen::Statement NodeBase::generateWrite(CodeGenerator &generator, uint32_t receiverIndex, std::unique_ptr<CodeGeneratorData> &data) const
    {
        throw 0;
    }

    void NodeBase::setup()
    {
        mFlowOutPins.resize(flowOutCount());
        mDataInPins.resize(dataInCount());
        mDataOutPins.resize(dataOutCount());

        mFlowInPins.resize(flowInCount());
        mDataProviderPins.resize(dataProviderCount());
        mDataReceiverPins.resize(dataReceiverCount());
    }

}
}

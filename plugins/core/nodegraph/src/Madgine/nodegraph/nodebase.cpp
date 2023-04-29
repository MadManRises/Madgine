#include "../nodegraphlib.h"

#include "nodebase.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "nodegraph.h"

#include "nodes/util/sendernode.h"

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

    const std::vector<Pin> &NodeBase::flowInSources(uint32_t index) const
    {
        return mFlowInPins[index].mSources;
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

    const std::vector<Pin> &NodeBase::dataReceiverSources(uint32_t index) const
    {
        return mDataReceiverPins[index].mSources;
    }

    uint32_t NodeBase::dataReceiverId(uint32_t index)
    {
        return 4001 + index;
    }

    const std::vector<Pin> &NodeBase::dataProviderTargets(uint32_t index) const
    {
        return mDataProviderPins[index].mTargets;
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

    void NodeBase::removeFlowOutPin(uint32_t index)
    {
        mGraph.onFlowOutRemove(this, index);
        if (mFlowOutPins[index].mTarget)
            mGraph.disconnectFlow({ mGraph.nodeIndex(this), index });
        mFlowOutPins.erase(mFlowOutPins.begin() + index);
    }

    void NodeBase::removeFlowInPin(uint32_t index)
    {
        mGraph.onFlowInRemove(this, index);
        while (!mFlowInPins[index].mSources.empty()) {
            mGraph.disconnectFlow(mFlowInPins[index].mSources.front());
        }
        mFlowInPins.erase(mFlowInPins.begin() + index);
    }

    void NodeBase::removeDataInPin(uint32_t index)
    {
        mGraph.onDataInRemove(this, index);
        if (mDataInPins[index].mSource)
            mGraph.disconnectDataIn({ mGraph.nodeIndex(this), index });
        mDataInPins.erase(mDataInPins.begin() + index);
    }

    void NodeBase::removeDataProviderPin(uint32_t index)
    {
        mGraph.onDataProviderRemove(this, index);
        while (!mDataProviderPins[index].mTargets.empty()) {
            mGraph.disconnectDataIn(mDataProviderPins[index].mTargets.front());
        }
        mDataProviderPins.erase(mDataProviderPins.begin() + index);
    }

    void NodeBase::removeDataOutPin(uint32_t index)
    {
        mGraph.onDataOutRemove(this, index);
        if (mDataOutPins[index].mTarget)
            mGraph.disconnectDataOut({ mGraph.nodeIndex(this), index });
        mDataOutPins.erase(mDataOutPins.begin() + index);
    }

    void NodeBase::removeDataReceiverPin(uint32_t index)
    {
        mGraph.onDataReceiverRemove(this, index);
        while (!mDataReceiverPins[index].mSources.empty()) {
            mGraph.disconnectDataOut(mDataReceiverPins[index].mSources.front());
        }
        mDataReceiverPins.erase(mDataReceiverPins.begin() + index);
    }

    
    void NodeBase::onFlowInRemove(Pin pin)
    {
        for (FlowInPinPrototype &inPin : mFlowInPins) {
            for (Pin &source : inPin.mSources) {
                if (source.mNode == pin.mNode) {
                    if (source && source.mIndex > pin.mIndex) {
                        --source.mIndex;
                    }
                }
            }
        }
    }

    void NodeBase::onFlowOutRemove(Pin pin)
    {
        for (FlowOutPinPrototype &outPin : mFlowOutPins) {
            Pin &target = outPin.mTarget;
            if (target.mNode == pin.mNode) {
                if (target && target.mIndex > pin.mIndex) {
                    --target.mIndex;
                }
            }
        }
    }

    
    void NodeBase::onDataOutRemove(Pin pin)
    {
        for (DataReceiverPinPrototype &receiver : mDataReceiverPins) {
            for (Pin &source : receiver.mSources) {
                if (source.mNode == pin.mNode) {
                    if (source && source.mIndex > pin.mIndex) {
                        --source.mIndex;
                    }
                }
            }
        }
    }

    void NodeBase::onDataReceiverRemove(Pin pin)
    {
        for (DataOutPinPrototype &outPin : mDataOutPins) {
            Pin &target = outPin.mTarget;
            if (target.mNode == pin.mNode) {
                if (target && target.mIndex > pin.mIndex) {
                    --target.mIndex;
                }
            }
        }
    }


    void NodeBase::onDataInRemove(Pin pin)
    {
        for (DataProviderPinPrototype &provider : mDataProviderPins) {
            for (Pin &target : provider.mTargets) {
                if (target.mNode == pin.mNode) {
                    if (target && target.mIndex > pin.mIndex) {
                        --target.mIndex;
                    }
                }
            }
        }
    }

    void NodeBase::onDataProviderRemove(Pin pin)
    {
        for (DataInPinPrototype &inPin : mDataInPins) {
            Pin &source = inPin.mSource;
            if (source.mNode == pin.mNode) {
                if (source && source.mIndex > pin.mIndex) {
                    --source.mIndex;
                }
            }
        }
    }

    void NodeBase::onNodeReindex(uint32_t oldIndex, uint32_t newIndex)
    {
        for (FlowOutPinPrototype &pin : mFlowOutPins) {
            assert(pin.mTarget.mNode != newIndex);
            if (pin.mTarget.mNode == oldIndex)
                pin.mTarget.mNode = newIndex;
        }
        for (DataInPinPrototype &pin : mDataInPins) {
            assert(pin.mSource.mNode != newIndex);
            if (pin.mSource.mNode == oldIndex)
                pin.mSource.mNode = newIndex;
        }
        for (DataOutPinPrototype &pin : mDataOutPins) {
            assert(pin.mTarget.mNode != newIndex);
            if (pin.mTarget.mNode == oldIndex)
                pin.mTarget.mNode = newIndex;
        }

        for (FlowInPinPrototype &pin : mFlowInPins) {
            for (Pin &source : pin.mSources) {
                assert(source.mNode != newIndex);
                if (source.mNode == oldIndex)
                    source.mNode = newIndex;
            }
        }
        for (DataReceiverPinPrototype &pin : mDataReceiverPins) {
            for (Pin &source : pin.mSources) {
                assert(source.mNode != newIndex);
                if (source.mNode == oldIndex)
                    source.mNode = newIndex;
            }
        }
        for (DataProviderPinPrototype &pin : mDataProviderPins) {
            for (Pin &target : pin.mTargets) {
                assert(target.mNode != newIndex);
                if (target.mNode == oldIndex)
                    target.mNode = newIndex;
            }
        }
    }

    void NodeBase::setupInterpret(NodeInterpreter &interpreter, std::unique_ptr<NodeInterpreterData> &data) const
    {
    }

    void NodeBase::interpret(NodeReceiver receiver, uint32_t flowIn, std::unique_ptr<NodeInterpreterData> &data) const
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

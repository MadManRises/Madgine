#include "../nodegraphlib.h"

#include "nodeprototypebase.h"

#include "Modules/serialize/streams/serializestream.h"

#include "Modules/serialize/serializetable_impl.h"

#include "Modules/keyvalueutil/valuetypeserialize.h"

SERIALIZETABLE_BEGIN(Engine::NodeGraph::NodePrototypeBase)
FIELD(mFlowOutPins)
FIELD(mDataInPins)
SERIALIZETABLE_END(Engine::NodeGraph::NodePrototypeBase)

namespace Engine {
namespace NodeGraph {

    TargetPin::operator bool() const
    {
        return mNode != nullptr;
    }

    uint32_t NodePrototypeBase::flowInId(uint32_t index)
    {
        return 1 + index;
    }

    size_t NodePrototypeBase::flowOutCount() const
    {
        return mFlowOutPins.size();
    }

    TargetPin NodePrototypeBase::flowOutTarget(uint32_t index) const
    {
        return mFlowOutPins[index].mTarget;
    }

    uint32_t NodePrototypeBase::flowOutId(uint32_t index)
    {
        return 1001 + index;
    }

    size_t NodePrototypeBase::dataInCount() const
    {
        return mDataInPins.size();
    }

    TargetPin NodePrototypeBase::dataInSource(uint32_t index) const
    {
        return mDataInPins[index].mSource;
    }

    ValueType &NodePrototypeBase::dataInDefault(uint32_t index)
    {
        return mDataInPins[index].mDefaultValue;
    }

    uint32_t NodePrototypeBase::dataInId(uint32_t index)
    {
        return 2001 + index;
    }

    size_t NodePrototypeBase::dataOutCount() const
    {
        return mDataOutPins.size();
    }

    ExtendedValueTypeDesc NodePrototypeBase::dataOutType(uint32_t index) const
    {
        return mDataOutPins[index].mType;
    }

    uint32_t NodePrototypeBase::dataOutId(uint32_t index)
    {
        return 3001 + index;
    }

    PinDesc NodePrototypeBase::fromId(uint32_t id)
    {
        PinDesc desc;
        desc.mDir = ((id % 2000) < 1000 ? PinDir::In : PinDir::Out);
        desc.mType = (id < 2000 ? PinType::Flow : PinType::Data);
        desc.mIndex = id % 1000 - 1;
        return desc;
    }

    void NodePrototypeBase::connectFlow(uint32_t index, GraphExecutable *target, uint32_t targetIndex)
    {
        mFlowOutPins[index].mTarget = { target, targetIndex };
    }

    void NodePrototypeBase::disconnectFlow(uint32_t index)
    {
        mFlowOutPins[index].mTarget = {};
    }

    void NodePrototypeBase::connectData(uint32_t index, GraphExecutable *source, uint32_t sourceIndex)
    {
        mDataInPins[index].mSource = { source, sourceIndex };
    }

    void NodePrototypeBase::disconnectData(uint32_t index)
    {
        mDataInPins[index].mSource = {};
    }

}
}

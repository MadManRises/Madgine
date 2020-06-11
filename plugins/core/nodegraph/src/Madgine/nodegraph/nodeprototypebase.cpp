#include "../nodegraphlib.h"

#include "nodeprototypebase.h"

#include "Modules/reflection/classname.h"

#include "Modules/serialize/streams/serializestream.h"

#include "Modules/serialize/serializetable_impl.h"

#include "Modules/keyvalueserialize/valuetypeserialize.h"

SERIALIZETABLE_BEGIN(Engine::NodeGraph::NodePrototypeBase)
FIELD(mFlowOutPins)
FIELD(mDataInPins)
SERIALIZETABLE_END(Engine::NodeGraph::NodePrototypeBase)

RegisterType(Engine::NodeGraph::NodePrototypeBase);

namespace Engine {
namespace NodeGraph {

    TargetPin::operator bool() const
    {
        return mNode != nullptr;
    }

    uintptr_t NodePrototypeBase::flowInId(size_t index)
    {
        return 1 + index;
    }

    size_t NodePrototypeBase::flowOutCount() const
    {
        auto [a, b] = mDataInPins[0];
        return mFlowOutPins.size();
    }

    TargetPin NodePrototypeBase::flowOutTarget(size_t index) const
    {
        return mFlowOutPins[index].mTarget;
    }

    uintptr_t NodePrototypeBase::flowOutId(size_t index)
    {
        return 1001 + index;
    }

    size_t NodePrototypeBase::dataInCount() const
    {
        return mDataInPins.size();
    }

    TargetPin NodePrototypeBase::dataInSource(size_t index) const
    {
        return mDataInPins[index].mSource;
    }

    ValueType &NodePrototypeBase::dataInDefault(size_t index)
    {
        return mDataInPins[index].mDefaultValue;
    }

    uintptr_t NodePrototypeBase::dataInId(size_t index)
    {
        return 2001 + index;
    }

    size_t NodePrototypeBase::dataOutCount() const
    {
        return mDataOutPins.size();
    }

    ExtendedValueTypeDesc NodePrototypeBase::dataOutType(size_t index) const
    {
        return mDataOutPins[index].mType;
    }

    uintptr_t NodePrototypeBase::dataOutId(size_t index)
    {
        return 3001 + index;
    }

    PinDesc NodePrototypeBase::fromId(uintptr_t id)
    {
        PinDesc desc;
        desc.mDir = ((id % 2000) < 1000 ? PinDir::In : PinDir::Out);
        desc.mType = (id < 2000 ? PinType::Flow : PinType::Data);
        desc.mIndex = id % 1000 - 1;
        return desc;
    }

    void NodePrototypeBase::connectFlow(size_t index, GraphExecutable *target, size_t targetIndex)
    {
        mFlowOutPins[index].mTarget = { target, targetIndex };
    }

    void NodePrototypeBase::disconnectFlow(size_t index)
    {
        mFlowOutPins[index].mTarget = {};
    }

    void NodePrototypeBase::connectData(size_t index, GraphExecutable *source, size_t sourceIndex)
    {
        mDataInPins[index].mSource = { source, sourceIndex };
    }

    void NodePrototypeBase::disconnectData(size_t index)
    {
        mDataInPins[index].mSource = {};
    }

}
}

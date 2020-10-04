#include "../moduleslib.h"
#include "serializableunit.h"

#include "serializemanager.h"

#include "toplevelunit.h"

#include "streams/bufferedstream.h"

#include "streams/operations.h"

#include "formatter.h"

namespace Engine {
namespace Serialize {

    SerializableUnitBase::SerializableUnitBase()
    {
    }

    SerializableUnitBase::SerializableUnitBase(const SerializableUnitBase &other)
        : SerializableUnitBase()
    {
    }

    SerializableUnitBase::SerializableUnitBase(SerializableUnitBase &&other) noexcept
        : mTopLevel(std::exchange(other.mTopLevel, nullptr))
        , mActiveIndex(std::exchange(other.mActiveIndex, 0))
        , mSynced(std::exchange(other.mSynced, false))
        , mType(other.mType)        
    {
    }

    SerializableUnitBase::~SerializableUnitBase()
    {
        //If this fails, you forgot to add all Serializables to the SerializeTable.
        //Temporary solution! Proper solution: Move mSynced into Serializable (=> evtl default mActive to true)
        assert(!mSynced);
    }

    SerializableUnitBase &SerializableUnitBase::operator=(const SerializableUnitBase &other)
    {
        return *this;
    }

    SerializableUnitBase &SerializableUnitBase::operator=(SerializableUnitBase &&other)
    {
        mTopLevel = std::exchange(other.mTopLevel, nullptr);
        mActiveIndex = std::exchange(other.mActiveIndex, 0);
        std::exchange(mSynced, other.mSynced);
        return *this;
    }

    void SerializableUnitBase::writeState(SerializeOutStream &out, const char *name, StateTransmissionFlags flags) const
    {
        out.format().beginCompound(out, name);
        mType->writeState(this, out);
        out.format().endCompound(out, name);
    }

    void SerializableUnitBase::readState(SerializeInStream &in, const char *name, StateTransmissionFlags flags)
    {
        in.format().beginCompound(in, name);
        mType->readState(this, in, flags);
        in.format().endCompound(in, name);
    }

    

    const TopLevelUnitBase *SerializableUnitBase::topLevel() const
    {
        return mTopLevel;
    }

    void SerializableUnitBase::setParent(SerializableUnitBase *parent)
    {
        if (mTopLevel != this)
            mTopLevel = parent ? parent->topLevel() : nullptr;
        mType->setParent(this);
    }

    void SerializableUnitBase::applySerializableMap(SerializeInStream &in)
    {
        mType->applySerializableMap(this, in);
    }

    void SerializableUnitBase::setSynced(bool b)
    {
        setDataSynced(b);
        setActive(b, true);
    }

    void SerializableUnitBase::setDataSynced(bool b)
    {
        assert(mSynced != b);
        mSynced = b;
        mType->setDataSynced(this, b);
    }

    void SerializableUnitBase::setActive(bool active, bool existenceChanged)
    {
        //assert(mSynced == active);
        mType->setActive(this, active, existenceChanged);
    }

    bool SerializableUnitBase::isActive(uint8_t index) const
    {
        return index < mActiveIndex;
    }

    void SerializableUnitBase::sync()
    {
        setSynced(true);
    }

    void SerializableUnitBase::unsync()
    {
        setSynced(false);
    }

    bool SerializableUnitBase::isSynced() const
    {
        return mSynced;
    }

    const SerializeTable *SerializableUnitBase::serializeType() const
    {
        return mType;
    }

} // namespace Serialize
} // namespace Core

#include "../moduleslib.h"
#include "serializableunit.h"

#include "streams/bufferedstream.h"

#include "serializemanager.h"

#include "toplevelserializableunit.h"

#include "syncable.h"

#include "serializeexception.h"

#include "serializable.h"

#include "../threading/threadlocal.h"

namespace Engine {
namespace Serialize {

    SerializableUnitBase::SerializableUnitBase(size_t masterId)
        : mMasterId(SerializeManager::generateMasterId(masterId, this))
    {
    }

    SerializableUnitBase::SerializableUnitBase(const SerializableUnitBase &other)
        : SerializableUnitBase()
    {
    }

    SerializableUnitBase::SerializableUnitBase(SerializableUnitBase &&other) noexcept
        : SerializableUnitBase()
    {
    }

    SerializableUnitBase::~SerializableUnitBase()
    {
		//If this fails, you forgot to add all Serializables to the SerializeTable.
		//Temporary solution! Proper solution: Move mSynced into Serializable (=> evtl default mActive to true)
        assert(!mSynced);

        clearSlaveId();
        SerializeManager::deleteMasterId(mMasterId, this);
    }

    void SerializableUnitBase::writeState(SerializeOutStream &out, const char *name, bool skipId) const
    {
        if (name)
			out.format().beginExtendedCompound(out, name);
        if (out.isMaster() && !skipId)
            this->writeId(out);
        if (name)
            out.format().beginCompound(out, name);
        mType->writeState(this, out);
        if (name)
			out.format().endCompound(out, name);
    }

    void SerializableUnitBase::writeId(SerializeOutStream &out) const
    {
        out.write(mMasterId, "id");
    }

    void SerializableUnitBase::readId(SerializeInStream &in)
    {
        size_t id;
        in.read(id, "id");
        if (topLevel() && in.manager() == topLevel()->getSlaveManager()) {
            setSlaveId(id);
        }
        if (mType->mIsTopLevelUnit)
            static_cast<TopLevelSerializableUnitBase *>(this)->setStaticSlaveId(id);
    }

    void SerializableUnitBase::readState(SerializeInStream &in, const char *name, bool skipId)
    {
        if (name)
			in.format().beginExtendedCompound(in, name);
        if (!in.isMaster() && !skipId)
            readId(in);
        if (name)
            in.format().beginCompound(in, name);
        mType->readState(this, in);
        if (name)
			in.format().endCompound(in, name);
    }

    void SerializableUnitBase::readAction(BufferedInOutStream &in)
    {
        size_t index;
        in >> index;
        //try { //TODO check error
        mType->readAction(this, in, index);
        /*reinterpret_cast<ObservableBase *>(reinterpret_cast<char *>(this) + index)
                ->readAction(in);*/
        /*} catch (const std::out_of_range &) {
            throw SerializeException("Unknown Observed-Id used! Possible binary mismatch!");
        }*/
    }

    void SerializableUnitBase::readRequest(BufferedInOutStream &in)
    {
        size_t index;
        in >> index;
        //try { //TODO check error
        mType->readRequest(this, in, index);
        /*reinterpret_cast<ObservableBase*>(reinterpret_cast<char *>(this) + index)
            ->readRequest(in);*/
        /*} catch (const std::out_of_range &) {
            throw SerializeException("Unknown Observed-Id used! Possible binary mismatch!");
        }*/
    }

    std::set<BufferedOutStream *, CompareStreamId> SerializableUnitBase::getMasterMessageTargets() const
    {
        if (mType->mIsTopLevelUnit) {
            return static_cast<const TopLevelSerializableUnitBase *>(this)->getMasterMessageTargets();
        }

        std::set<BufferedOutStream *, CompareStreamId> result;
        if (mSynced && mParent) {
            result = mParent->getMasterMessageTargets();
        }
        return result;
    }

    BufferedOutStream *SerializableUnitBase::getSlaveMessageTarget() const
    {
        BufferedOutStream *result = nullptr;
        if (mSynced) {
            result = topLevel()->getSlaveMessageTarget();
        }
        return result;
    }

    void SerializableUnitBase::writeCreationData(SerializeOutStream &out) const
    {
    }

    const TopLevelSerializableUnitBase *SerializableUnitBase::topLevel() const
    {
        if (mType->mIsTopLevelUnit)
            return static_cast<const TopLevelSerializableUnitBase *>(this);
        else
            return mParent ? mParent->topLevel() : nullptr;
    }

    void SerializableUnitBase::clearSlaveId()
    {
        if (mSlaveId != 0) {
            topLevel()->getSlaveManager()->removeSlaveMapping(this);
            mSlaveId = 0;
        }
    }

    void SerializableUnitBase::setParent(SerializableUnitBase *parent)
    {
        clearSlaveId();
        mParent = parent;
    }

    size_t SerializableUnitBase::slaveId() const
    {
        return mSlaveId;
    }

    size_t SerializableUnitBase::masterId() const
    {
        return mMasterId;
    }

    void SerializableUnitBase::setSlaveId(size_t id)
    {
        if (mSlaveId != id) {
            if (mSlaveId != 0) {
                clearSlaveId();
            }
            mSlaveId = id;
            topLevel()->getSlaveManager()->addSlaveMapping(this);
        }
    }

    void SerializableUnitBase::applySerializableMap(const std::map<size_t, SerializableUnitBase *> &map)
    {
        mType->applySerializableMap(this, map);
    }

    void SerializableUnitBase::setSynced(bool b)
    {
        setDataSynced(b);
        setActive(b);
    }

    void SerializableUnitBase::setDataSynced(bool b)
    {
        assert(mSynced != b);
        mSynced = b;
        mType->setDataSynced(this, b);
    }

    void SerializableUnitBase::setActive(bool active)
    {
        assert(mSynced == active);
        mType->setActive(this, active);
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

    bool SerializableUnitBase::isMaster() const
    {
        return mSlaveId == 0;
    }

    const SerializeTable *SerializableUnitBase::type() const
    {
        return mType;
    }

    /*bool SerializableUnitBase::filter(SerializeOutStream *stream) const
    {
        return true;
    }*/
} // namespace Serialize
} // namespace Core

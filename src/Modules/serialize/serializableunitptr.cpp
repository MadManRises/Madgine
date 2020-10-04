#include "../moduleslib.h"

#include "serializableunitptr.h"

#include "streams/serializestream.h"

#include "formatter.h"

#include "serializableunit.h"

#include "streams/operations.h"

#include "serializemanager.h"

#include "toplevelserializableunit.h"

namespace Engine {
namespace Serialize {

	 void SerializableUnitPtr::writeState(SerializeOutStream &out, const char *name, StateTransmissionFlags flags) const
    {
        if (out.isMaster() && !(flags & StateTransmissionFlags_SkipId)) {
            out.format().beginExtended(out, name, 1);
            write(out, mUnit->mMasterId, "id");
        }
        out.format().beginCompound(out, name);
        mType->writeState(mUnit, out);
        out.format().endCompound(out, name);
    }

    void SerializableUnitPtr::readState(SerializeInStream &in, const char *name, StateTransmissionFlags flags)
    {
        if (!in.isMaster() && !(flags & StateTransmissionFlags_SkipId)) {
            in.format().beginExtended(in, name, 1);
            UnitId id;
            read(in, id, "id");

            if (in.manager() && in.manager()->getSlaveStreambuf() == &in.buffer()) {
                mUnit->setSlaveId(id, in.manager());
                if (mType->mIsTopLevelUnit) {
                    static_cast<TopLevelSerializableUnitBase *>(mUnit)->setStaticSlaveId(id);
                }
            }
        }
        in.format().beginCompound(in, name);
        mType->readState(mUnit, in, flags);
        in.format().endCompound(in, name);
    }

    void SerializableUnitPtr::writeAction(uint8_t index, int op, const void *data, ParticipantId answerTarget, TransactionId answerId) const
    {
        mType->writeAction(mUnit, index, op, data, answerTarget, answerId);
    }

    void SerializableUnitPtr::readAction(BufferedInOutStream &in, PendingRequest *request)
    {
        mType->readAction(mUnit, in, request);
    }

    void SerializableUnitPtr::writeRequest(uint8_t index, int op, const void *data, ParticipantId requester, TransactionId requesterTransactionId, std::function<void(void *)> callback) const
    {
        mType->writeRequest(mUnit, index, op, data, requester, requesterTransactionId, callback);
    }

    void SerializableUnitPtr::readRequest(BufferedInOutStream &in, TransactionId id)
    {
        mType->readRequest(mUnit, in, id);
    }

    void SerializableUnitPtr::setParent(SerializableUnitBase *parent)
    {
        if (mUnit->mTopLevel != mUnit)
            mUnit->mTopLevel = parent ? parent->topLevel() : nullptr;
        mType->setParent(mUnit);
    }

    void SerializableUnitPtr::applySerializableMap(SerializeInStream &in)
    {
        mType->applySerializableMap(mUnit, in);
    }

    void SerializableUnitPtr::setSynced(bool b)
    {
        setDataSynced(b);
        setActive(b, true);
    }

    void SerializableUnitPtr::setDataSynced(bool b)
    {
        assert(mUnit->mSynced != b);
        mUnit->mSynced = b;
        mType->setDataSynced(mUnit, b);
    }

    void SerializableUnitPtr::setActive(bool active, bool existenceChanged)
    {
        //assert(mSynced == active);
        mType->setActive(mUnit, active, existenceChanged);
    }

    void SerializableUnitPtr::sync()
    {
        setSynced(true);
    }

    void SerializableUnitPtr::unsync()
    {
        setSynced(false);
    }


}
}
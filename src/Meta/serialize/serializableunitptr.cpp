#include "../metalib.h"

#include "serializableunitptr.h"

#include "streams/serializestream.h"

#include "formatter.h"

#include "serializableunit.h"

#include "streams/operations.h"

#include "serializemanager.h"

#include "toplevelunit.h"

#include "streams/serializablemapholder.h"

namespace Engine {
namespace Serialize {

    void SerializableDataConstPtr::writeState(SerializeOutStream &out, const char *name, CallerHierarchyBasePtr hierarchy, StateTransmissionFlags flags) const
    {
        SerializableMapHolder holder;
        out.startSerializableWrite(&holder);

        if (out.isMaster() && !(flags & StateTransmissionFlags_SkipId)) {
            out.format().beginExtended(out, name, 1);
            write(out, mUnit, "serId");
        }

        out.format().beginCompound(out, name);
        mType->writeState(mUnit, out, hierarchy);
        out.format().endCompound(out, name);
    }

    void SerializableDataPtr::readState(SerializeInStream &in, const char *name, CallerHierarchyBasePtr hierarchy, StateTransmissionFlags flags) const
    {
        SerializableListHolder holder;
        in.startSerializableRead(&holder);

        if (!in.isMaster() && !(flags & StateTransmissionFlags_SkipId)) {
            in.format().beginExtended(in, name, 1);
            SerializableUnitBase *idHelper;
            read(in, idHelper, "serId");
            uint32_t id = reinterpret_cast<uintptr_t>(idHelper) >> 2;
            SerializableUnitList &list = in.serializableList();
            if (list.size() <= id)
                list.resize(id + 1);
            assert(!list[id]);
            list[id] = unit();
        }

        in.format().beginCompound(in, name);
        mType->readState(unit(), in, flags, hierarchy);
        in.format().endCompound(in, name);
    }

    SerializableDataPtr::SerializableDataPtr(const SerializableUnitPtr &other)
        : SerializableDataPtr(other.unit(), other.mType)
    {
    }

    SerializableDataUnit *SerializableDataPtr::unit() const
    {
        return const_cast<SerializableDataUnit *>(mUnit);
    }

    const SerializableUnitBase *SerializableUnitConstPtr::unit() const
    {
        return static_cast<const SerializableUnitBase *>(mUnit);
    }

    SerializableUnitConstPtr::SerializableUnitConstPtr(const SerializableUnitBase *unit, const SerializeTable *type)
        : SerializableDataConstPtr { unit, type }
    {
    }

    bool SerializableUnitConstPtr::isActive(size_t offset) const
    {
        //TODO: Maybe save lookup -> enforce order of elements in memory
        return mType->getIndex(offset) < unit()->mActiveIndex;
    }

    void SerializableUnitPtr::readState(SerializeInStream &in, const char *name, CallerHierarchyBasePtr hierarchy, StateTransmissionFlags flags) const
    {
        SerializableListHolder holder;
        in.startSerializableRead(&holder);

        if (!in.isMaster() && !(flags & StateTransmissionFlags_SkipId)) {
            in.format().beginExtended(in, name, 1);
            SerializableUnitBase *idHelper;
            read(in, idHelper, "serId");
            uint32_t id = reinterpret_cast<uintptr_t>(idHelper) >> 2;
            SerializableUnitList &list = in.serializableList();
            if (list.size() <= id)
                list.resize(id + 1);
            assert(!list[id]);
            list[id] = unit();
        }

        in.format().beginCompound(in, name);
        mType->readState(unit(), in, flags, hierarchy);
        in.format().endCompound(in, name);
    }

    void SerializableUnitConstPtr::writeAction(uint8_t index, const std::set<BufferedOutStream *, CompareStreamId> &outStreams, const void *data) const
    {
        mType->writeAction(unit(), index, outStreams, data);
    }

    void SerializableUnitPtr::readAction(BufferedInOutStream &in, PendingRequest *request) const
    {
        mType->readAction(unit(), in, request);
    }

    void SerializableUnitConstPtr::writeRequest(uint8_t index, BufferedOutStream &out, const void *data) const
    {
        mType->writeRequest(unit(), index, out, data);
    }

    void SerializableUnitPtr::readRequest(BufferedInOutStream &in, TransactionId id) const
    {
        mType->readRequest(unit(), in, id);
    }

    void SerializableUnitPtr::setParent(SerializableUnitBase *parent) const
    {
        if (unit()->mTopLevel != mUnit)
            unit()->mTopLevel = parent ? parent->mTopLevel : nullptr;
        mType->setParent(unit());
    }

    void SerializableDataPtr::applySerializableMap(SerializeInStream &in) const
    {
        mType->applySerializableMap(unit(), in);
    }

    void SerializableUnitPtr::setDataSynced(bool b) const
    {
        assert(unit()->mSynced != b);
        unit()->mSynced = b;
        mType->setDataSynced(unit(), b);
    }

    void SerializableUnitPtr::setActive(bool active, bool existenceChanged) const
    {
        //assert(mSynced == active);
        mType->setActive(unit(), active, existenceChanged);
    }

    SerializableUnitBase *SerializableUnitPtr::unit() const
    {
        return const_cast<SerializableUnitBase *>(SerializableUnitConstPtr::unit());
    }
}
}
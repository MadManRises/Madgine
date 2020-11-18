#pragma once

#include "serializableunit.h"

#include "streams/comparestreamid.h"

namespace Engine {
namespace Serialize {

#define SYNCABLEUNIT_MEMBERS()            \
    SERIALIZABLEUNIT_MEMBERS()            \
    READONLY_PROPERTY(MasterId, masterId) \
    READONLY_PROPERTY(SlaveId, slaveId)

    struct MODULES_EXPORT SyncableUnitBase : SerializableUnitBase {
    protected:
        SyncableUnitBase(UnitId masterId = 0);
        SyncableUnitBase(const SyncableUnitBase &other);
        SyncableUnitBase(SyncableUnitBase &&other) noexcept;
        ~SyncableUnitBase();

        SyncableUnitBase &operator=(const SyncableUnitBase &other);
        SyncableUnitBase &operator=(SyncableUnitBase &&other);

    public:
        void writeState(SerializeOutStream &out, const char *name = nullptr, StateTransmissionFlags flags = 0, CallerHierarchyBasePtr hierarchy = {}) const;
        void readState(SerializeInStream &in, const char *name = nullptr, StateTransmissionFlags flags = 0, CallerHierarchyBasePtr hierarchy = {});

        void readAction(BufferedInOutStream &in, PendingRequest *request);
        void readRequest(BufferedInOutStream &in, TransactionId id);

        UnitId slaveId() const;
        UnitId masterId() const;

        bool isMaster() const;

    protected:
        void setSlaveId(UnitId id, SerializeManager *mgr);

        const SerializeTable *serializeType() const;

        UnitId moveMasterId(UnitId newId = 0);

    private:
        std::set<BufferedOutStream *, CompareStreamId> getMasterMessageTargets() const;
        BufferedOutStream *getSlaveMessageTarget() const;

        void clearSlaveId(SerializeManager *mgr);

        friend struct SyncManager;
        friend struct SerializeUnitHelper;
        friend struct SyncableBase;
        friend struct SerializeTable;
        template <typename T>
        friend struct UnitHelper;
        template <typename T, typename Base>
        friend struct TableInitializer;
        template <typename T>
        friend struct Syncable;
        template <typename T>
        friend struct Serializable;

        DERIVE_FRIEND(setDataSynced);
        DERIVE_FRIEND(setActive);
        DERIVE_FRIEND(setParent);

    private:
        UnitId mSlaveId = 0;
        UnitId mMasterId;

        const SerializeTable *mType = nullptr;
    };

    template <typename T, typename Base>
    struct SyncableUnit;

    template <typename T, typename Base>
    struct TableInitializer {
        TableInitializer()
        {
            static_cast<SyncableUnit<T, Base> *>(this)->mType = &serializeTable<T>();
        }
        TableInitializer(const TableInitializer &)
        {
            static_cast<SyncableUnit<T, Base> *>(this)->mType = &serializeTable<T>();
        }
        TableInitializer(TableInitializer &&)
        {
            static_cast<SyncableUnit<T, Base> *>(this)->mType = &serializeTable<T>();
        }
        TableInitializer &operator=(const TableInitializer &)
        {
            return *this;
        }
        TableInitializer &operator=(TableInitializer &&)
        {
            return *this;
        }
    };

    template <typename T, typename _Base = SyncableUnitBase>
    struct SyncableUnit : _Base, private TableInitializer<T, _Base> {
    protected:
        friend TableInitializer<T, _Base>;

        using _Base::_Base;
    };

} // namespace Serialize
} // namespace Core

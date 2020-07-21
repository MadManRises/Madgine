#pragma once

#include "streams/comparestreamid.h"

#include "serializetable.h"

#include "statetransmissionflags.h"

namespace Engine {
namespace Serialize {

#define SERIALIZABLEUNIT_MEMBERS()        \
    READONLY_PROPERTY(Synced, isSynced)   \
    READONLY_PROPERTY(MasterId, masterId) \
    READONLY_PROPERTY(SlaveId, slaveId)

    struct MODULES_EXPORT SerializableUnitBase {
    protected:
        SerializableUnitBase(UnitId masterId = 0);
        SerializableUnitBase(const SerializableUnitBase &other);
        SerializableUnitBase(SerializableUnitBase &&other) noexcept;
        ~SerializableUnitBase();

    public:
        const TopLevelSerializableUnitBase *topLevel() const;

        void writeState(SerializeOutStream &out, const char *name = nullptr, StateTransmissionFlags flags = 0) const;
        void readState(SerializeInStream &in, const char *name = nullptr, StateTransmissionFlags flags = 0);

        void readAction(BufferedInOutStream &in, PendingRequest *request);
        void readRequest(BufferedInOutStream &in, TransactionId id);

        void applySerializableMap(SerializeInStream &in);

        UnitId slaveId() const;
        UnitId masterId() const;

        bool isSynced() const;
        bool isMaster() const;

        void swap(SerializableUnitBase &other);

    protected:
        void sync();
        void unsync();

        void setSynced(bool b);

        void setSlaveId(UnitId id, SerializeManager *mgr);

        const SerializeTable *serializeType() const;

        UnitId moveMasterId(UnitId newId = 0);

    private:
        std::set<BufferedOutStream *, CompareStreamId> getMasterMessageTargets() const;
        BufferedOutStream *getSlaveMessageTarget() const;

        void clearSlaveId(SerializeManager *mgr);

        void setDataSynced(bool b);
        void setActive(bool active, bool existenceChanged);
        void setParent(SerializableUnitBase *parent);

        bool isActive(uint8_t index) const;

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
        SerializableUnitBase *mParent = nullptr;

        UnitId mSlaveId = 0;
        UnitId mMasterId;
        uint8_t mActiveIndex = 0;

        bool mSynced = false; // Maybe move only into TopLevelUnit?

        const SerializeTable *mType = nullptr;
    };

    template <typename T, typename Base>
    struct SerializableUnit;

    template <typename T, typename Base>
    struct TableInitializer {
        TableInitializer()
        {
            static_cast<SerializableUnit<T, Base> *>(this)->mType = &serializeTable<T>();
        }
        TableInitializer(const TableInitializer &)
        {
            static_cast<SerializableUnit<T, Base> *>(this)->mType = &serializeTable<T>();
        }
        TableInitializer(TableInitializer &&)
        {
            static_cast<SerializableUnit<T, Base> *>(this)->mType = &serializeTable<T>();
        }
    };

    template <typename T, typename _Base = SerializableUnitBase>
    struct SerializableUnit : _Base, private TableInitializer<T, _Base> {
    protected:
        friend TableInitializer<T, _Base>;

        typedef T Self;

        using _Base::_Base;
    };

    template <typename>
    struct __SerializeInstance;

#define SERIALIZABLEUNIT                                          \
    friend struct ::Engine::Serialize::__SerializeInstance<Self>; \
    friend struct ::Engine::Serialize::SerializeTableCallbacks;   \
    DERIVE_FRIEND(onActivate, ::Engine::Serialize::)                                     \

} // namespace Serialize
} // namespace Core

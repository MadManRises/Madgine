#pragma once

#include "streams/comparestreamid.h"

#include "serializetable.h"

#include "statesubmissionflags.h"

namespace Engine {
namespace Serialize {
    namespace __serialized__impl__ {
        template <class T>
        class SerializedUnit;
    }

    class MODULES_EXPORT SerializableUnitBase {
    protected:
        SerializableUnitBase(size_t masterId = 0);
        SerializableUnitBase(const SerializableUnitBase &other);
        SerializableUnitBase(SerializableUnitBase &&other) noexcept;
        ~SerializableUnitBase();

    public:
        const TopLevelSerializableUnitBase *topLevel() const;

        void writeState(SerializeOutStream &out, const char *name = nullptr, StateTransmissionFlags flags = 0) const;
        void readState(SerializeInStream &in, const char *name = nullptr, StateTransmissionFlags flags = 0);

        void readAction(BufferedInOutStream &in);
        void readRequest(BufferedInOutStream &in);

        void writeId(SerializeOutStream &out) const;
        void readId(SerializeInStream &in);

        void applySerializableMap(const std::map<size_t, SerializableUnitBase *> &map);

        void writeCreationData(SerializeOutStream &out) const;

        size_t slaveId() const;
        size_t masterId() const;

        bool isSynced() const;
        bool isMaster() const;

    protected:
        void sync();
        void unsync();

        void setSynced(bool b);

        void setSlaveId(size_t id, SerializeManager *mgr);

		const SerializeTable *type() const;

    private:
        std::set<BufferedOutStream *, CompareStreamId> getMasterMessageTargets() const;
        BufferedOutStream *getSlaveMessageTarget() const;

        void clearSlaveId(SerializeManager *mgr);

        void setDataSynced(bool b);
        void setActive(bool active);


        friend struct SyncManager;        
        friend struct SerializeUnitHelper;
        friend struct SyncableBase;
        friend struct SerializeTable;
		template <typename T, bool b>
        friend struct UnitHelper;
        template <typename T, typename Base>
        friend struct TableInitializer;
		template <typename T>
        friend struct Syncable;
		
    protected:
        void setParent(SerializableUnitBase *parent);

    private:
        SerializableUnitBase *mParent = nullptr;

        size_t mSlaveId = 0;
        size_t mMasterId;

        bool mSynced = false;

		const SerializeTable *mType = nullptr;
	};

	template <typename T, typename Base>
    struct SerializableUnit;

	template <typename T, typename Base>
        struct TableInitializer {
			template <typename... Args>
            TableInitializer(Args&&...)
            {
                static_cast<SerializableUnit<T, Base> *>(this)->mType = &serializeTable<T>();
            }
		};

    template <typename T, typename Base = SerializableUnitBase>
    struct SerializableUnit : Base, private TableInitializer<T, Base> {
    protected:
        friend struct TableInitializer<T, Base>;

        typedef T Self;
		
		using Base::Base;       

    };


	template <typename>
    struct __SerializeInstance;

	#define SERIALIZABLEUNIT friend struct ::Engine::Serialize::__SerializeInstance<Self>

} // namespace Serialize
} // namespace Core


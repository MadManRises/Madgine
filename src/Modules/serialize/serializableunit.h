#pragma once

#include "streams/comparestreamid.h"

#include "serializetable.h"

#include "statetransmissionflags.h"

#include "serializabledataunit.h"

namespace Engine {
namespace Serialize {

#define SERIALIZABLEUNIT_MEMBERS() \
    READONLY_PROPERTY(Synced, isSynced)

    struct MODULES_EXPORT SerializableUnitBase : SerializableDataUnit {
    protected:
        SerializableUnitBase();
        SerializableUnitBase(const SerializableUnitBase &other);
        SerializableUnitBase(SerializableUnitBase &&other) noexcept;
        ~SerializableUnitBase();

        SerializableUnitBase &operator=(const SerializableUnitBase &other);
        SerializableUnitBase &operator=(SerializableUnitBase &&other);

    public:
        bool isSynced() const;

    protected:
        friend struct SyncableBase;
        friend struct SerializableUnitPtr;
        friend struct SerializableUnitConstPtr;
        friend struct TopLevelUnitBase;
        friend struct SerializeTable;
        template <typename>
        friend struct Serializable;
        friend struct SyncableUnitBase;
        friend struct SyncManager;

    private:
        const TopLevelUnitBase *mTopLevel = nullptr;

        uint8_t mActiveIndex = 0;

        bool mSynced = false;
    };

} // namespace Serialize
} // namespace Core

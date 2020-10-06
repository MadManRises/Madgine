#pragma once

#include "streams/comparestreamid.h"

#include "serializetable.h"

#include "statetransmissionflags.h"

namespace Engine {
namespace Serialize {

#define SERIALIZABLEUNIT_MEMBERS() \
    READONLY_PROPERTY(Synced, isSynced)

    struct MODULES_EXPORT SerializableUnitBase {
    protected:
        SerializableUnitBase();
        SerializableUnitBase(const SerializableUnitBase &other);
        SerializableUnitBase(SerializableUnitBase &&other) noexcept;
        ~SerializableUnitBase();

        SerializableUnitBase &operator=(const SerializableUnitBase &other);
        SerializableUnitBase &operator=(SerializableUnitBase &&other);

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

    template <typename>
    struct __SerializeInstance;

#define SERIALIZABLEUNIT(_Self)                                    \
    friend struct ::Engine::Serialize::__SerializeInstance<_Self>; \
    friend struct ::Engine::Serialize::SerializeTableCallbacks;    \
    DERIVE_FRIEND(onActivate, ::Engine::Serialize::)               \
    using Self = _Self

} // namespace Serialize
} // namespace Core

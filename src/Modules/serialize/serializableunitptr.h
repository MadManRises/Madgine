#pragma once

#include "serializetable.h"

namespace Engine {
namespace Serialize {

    DERIVE_FUNCTION(customUnitPtr);

    struct MODULES_EXPORT SerializableUnitPtr {
    private:
        template <typename T>
        SerializableUnitPtr(T *t, std::true_type)
            : SerializableUnitPtr(t ? t->customUnitPtr() : SerializableUnitPtr { nullptr, serializeTable<decayed_t<T>> })
        {
        }

        template <typename T>
        SerializableUnitPtr(T *t, std::false_type)
            : mUnit(t)
            , mType(serializeTable<decayed_t<T>>)
        {
        }

    public:
        constexpr SerializableUnitPtr() = default;

        template <typename T, typename = std::enable_if_t<!std::is_same_v<SerializableUnitBase, T> && std::is_base_of_v<SerializableUnitBase, T>>>
        SerializableUnitPtr(T *t)
            : SerializableUnitPtr(t, has_function_customUnitPtr<T> {})
        {
        }

        constexpr SerializableUnitPtr(const SerializableUnitPtr &other)
            : mUnit(other.mUnit)
            , mType(other.mType)
        {
        }

        SerializableUnitPtr(SerializableUnitBase *unit, const SerializeTable *type)
            : mUnit(unit)
            , mType(type)
        {
        }

        bool operator==(const SerializableUnitPtr &other) const
        {
            return mUnit == other.mUnit && mType == other.mType;
        }

        bool operator<(const SerializableUnitPtr &other) const
        {
            if (mUnit < other.mUnit)
                return true;
            if (mUnit > other.mUnit)
                return false;
            return mType < other.mType;
        }

        /*template <typename T>
        T *safe_cast() const
        {
            if (!mType->isDerivedFrom<std::remove_const_t<T>>())
                std::terminate();
            return static_cast<T *>(mScope);
        }*/

        operator bool() const
        {
            return mUnit != nullptr;
        }

        void writeState(SerializeOutStream &out, const char *name = nullptr, StateTransmissionFlags flags = 0) const;
        void readState(SerializeInStream &in, const char *name = nullptr, StateTransmissionFlags flags = 0);

        void writeAction(uint8_t index, int op, const void *data, ParticipantId answerTarget, TransactionId answerId) const;
        void readAction(BufferedInOutStream &in, PendingRequest *request);
        void writeRequest(uint8_t index, int op, const void *data, ParticipantId requester, TransactionId requesterTransactionId, std::function<void(void *)> callback) const;
        void readRequest(BufferedInOutStream &in, TransactionId id);

        void applySerializableMap(SerializeInStream &in);

        void sync();
        void unsync();

        void setSynced(bool b);

        void setDataSynced(bool b);
        void setActive(bool active, bool existenceChanged);
        void setParent(SerializableUnitBase *parent);

        SerializableUnitBase *mUnit = nullptr;
        const SerializeTable *mType = nullptr;
    };

}
}
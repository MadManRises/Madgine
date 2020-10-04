#pragma once

#include "serializetable.h"

namespace Engine {
namespace Serialize {

    DERIVE_FUNCTION(customUnitPtr);

    struct MODULES_EXPORT SerializableUnitConstPtr {
    private:
        template <typename T>
        SerializableUnitConstPtr(const T *t, std::true_type)
            : SerializableUnitConstPtr(t ? t->customUnitPtr() : SerializableUnitConstPtr { nullptr, &serializeTable<decayed_t<T>>() })
        {
        }

        template <typename T>
        SerializableUnitConstPtr(const T *t, std::false_type)
            : mUnit(t)
            , mType(&serializeTable<decayed_t<T>>())
        {
        }

    public:
        constexpr SerializableUnitConstPtr() = default;

        template <typename T, typename = std::enable_if_t<!std::is_same_v<SerializableUnitBase, T> && std::is_base_of_v<SerializableUnitBase, T>>>
        SerializableUnitConstPtr(const T *t)
            : SerializableUnitConstPtr(t, has_function_customUnitPtr<T> {})
        {
        }

        constexpr SerializableUnitConstPtr(const SerializableUnitConstPtr &other)
            : mUnit(other.mUnit)
            , mType(other.mType)
        {
        }

        SerializableUnitConstPtr(const SerializableUnitBase *unit, const SerializeTable *type)
            : mUnit(unit)
            , mType(type)
        {
        }

        bool operator==(const SerializableUnitConstPtr &other) const
        {
            return mUnit == other.mUnit && mType == other.mType;
        }

        bool operator<(const SerializableUnitConstPtr &other) const
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

        bool isActive(size_t offset) const;

        void writeState(SerializeOutStream &out, const char *name = nullptr, StateTransmissionFlags flags = 0) const;

        void writeAction(uint8_t index, int op, const void *data, ParticipantId answerTarget, TransactionId answerId) const;
        void writeRequest(uint8_t index, int op, const void *data, ParticipantId requester, TransactionId requesterTransactionId, std::function<void(void *)> callback) const;

        const SerializableUnitBase *mUnit = nullptr;
        const SerializeTable *mType = nullptr;
    };

    struct MODULES_EXPORT SerializableUnitPtr : SerializableUnitConstPtr {
    private:
        template <typename T>
        SerializableUnitPtr(T *t, std::true_type)
            : SerializableUnitPtr(t ? t->customUnitPtr() : SerializableUnitPtr { nullptr, &serializeTable<decayed_t<T>>() })
        {
        }

        template <typename T>
        SerializableUnitPtr(T *t, std::false_type)
            : SerializableUnitConstPtr(t)
        {
        }

    public:
        constexpr SerializableUnitPtr() = default;

        template <typename T, typename = std::enable_if_t<!std::is_same_v<SerializableUnitBase, T> && std::is_base_of_v<SerializableUnitBase, T> && !std::is_const_v<T>>>
        SerializableUnitPtr(T *t)
            : SerializableUnitPtr(t, has_function_customUnitPtr<T> {})
        {
        }

        constexpr SerializableUnitPtr(const SerializableUnitPtr &other) = default;

        SerializableUnitPtr(SerializableUnitBase *unit, const SerializeTable *type)
            : SerializableUnitConstPtr(unit, type)
        {
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

        void readState(SerializeInStream &in, const char *name = nullptr, StateTransmissionFlags flags = 0) const;

        void readAction(BufferedInOutStream &in, PendingRequest *request) const;
        void readRequest(BufferedInOutStream &in, TransactionId id) const;

        void applySerializableMap(SerializeInStream &in) const;

        void sync() const;
        void unsync() const;

        void setSynced(bool b) const;

        void setDataSynced(bool b) const;
        void setActive(bool active, bool existenceChanged) const;
        void setParent(SerializableUnitBase *parent) const;

        SerializableUnitBase *unit() const;        
    };

}
}
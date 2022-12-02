#pragma once

#include "Generic/type_pack.h"

namespace Engine {
namespace Serialize {

    struct EnumTag;

    using SerializePrimitives = type_pack<
        bool,
        uint8_t,
        int8_t,
        uint16_t,
        int16_t,
        uint32_t,
        int32_t,
        uint64_t,
        int64_t,
        float,
        SyncableUnitBase *,
        SerializableDataUnit *,
        std::string,
        ByteBuffer,
        Void,
        Vector3,
        Vector4,
        Matrix3,
        EnumTag,
        Color3,
        Color4>;

    template <typename T, typename = void>
    struct PrimitiveReducer {
        typedef T type;
    };

    template <std::convertible_to<const SyncableUnitBase *> T>
    struct PrimitiveReducer<T> {
        typedef SyncableUnitBase *type;
    };

    template <typename T>
    concept SerializableUnitPtrHelper = !std::convertible_to<T, const SyncableUnitBase *> && std::convertible_to<T, const SerializableDataUnit *>;

    template <SerializableUnitPtrHelper T>
    struct PrimitiveReducer<T> {
        typedef SerializableDataUnit *type;
    };

    template <Enum T>
    struct PrimitiveReducer<T> {
        typedef std::underlying_type_t<T> type;
    };

    template <typename T>
    struct PrimitiveReducer<EnumType<T>> {
        typedef EnumTag type;
    };

    template <typename T, T invalid>
    struct PrimitiveReducer<IndexType<T, invalid>> {
        typedef T type;
    };

    template <String S>
    struct PrimitiveReducer<S> {
        typedef std::string type;
    };

    template <typename T>
    const constexpr size_t PrimitiveTypeIndex_v = SerializePrimitives::index<uint8_t, typename PrimitiveReducer<T>::type>;

    template <typename T>
    concept PrimitiveType = SerializePrimitives::contains<typename PrimitiveReducer<T>::type>;
}
}
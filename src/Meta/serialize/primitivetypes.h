#pragma once

#include "Generic/type_pack.h"

namespace Engine {
namespace Serialize {

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
        type_pack<std::string, std::string_view, CoWString>,
        ByteBuffer,
        std::monostate,
        Vector3,
        Vector4,
        Matrix3>;

    template <typename T, typename = void>
    struct PrimitiveReducer {
        typedef T type;
    };

    template <std::convertible_to<const SyncableUnitBase*> T>
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

    template <typename T, typename = void>
    struct PrimitiveTypeIndex : type_pack_index<uint8_t, SerializePrimitives, typename PrimitiveReducer<T>::type> {
    };

    template <typename T>
    const constexpr size_t PrimitiveTypeIndex_v = PrimitiveTypeIndex<T>::value;

    template <typename T, typename = void>
    struct PrimitiveTypesContain : type_pack_contains<SerializePrimitives, typename PrimitiveReducer<T>::type> {
    };

    template <typename T>
    const constexpr bool PrimitiveTypesContain_v = PrimitiveTypesContain<T>::value;

    template <typename T>
    const constexpr bool isPrimitiveType_v = PrimitiveTypesContain_v<T> || std::is_convertible_v<T, const SerializableDataUnit *> || std::is_convertible_v<T, const SyncableUnitBase *> || std::is_enum_v<T>;

}
}
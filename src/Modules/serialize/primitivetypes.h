#pragma once

#include "../generic/type_pack.h"

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
        SerializableUnitBase *, 
        std::string, 
        std::string_view,
        ByteBuffer, 
        std::monostate,
        Filesystem::Path,
        Matrix3
    >;

    template <typename T, typename = void>
    struct PrimitiveReducer {
        typedef T type;
    };

    template <typename T>
    struct PrimitiveReducer<T, std::enable_if_t<std::is_convertible_v<T, const SyncableUnitBase *>>> {
        typedef SyncableUnitBase *type;
    };

        template <typename T>
    struct PrimitiveReducer<T, std::enable_if_t<!std::is_convertible_v<T, const SyncableUnitBase *> && std::is_convertible_v<T, const SerializableUnitBase *>>> {
        typedef SerializableUnitBase *type;
    };

    template <typename T>
    struct PrimitiveReducer<T, std::enable_if_t<std::is_enum_v<T>>> {
        typedef int type;
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
    const constexpr bool isPrimitiveType_v = PrimitiveTypesContain_v<T> || std::is_convertible_v<T, const SerializableUnitBase *> || std::is_enum_v<T>;

}
}
#pragma once

namespace Engine {
namespace Serialize {

	using SerializePrimitives = type_pack<bool, size_t, int, float, SerializableUnitBase *, std::string, std::string_view, Filesystem::Path, Vector2, Vector3, Vector2i, Vector3i, Matrix3, ByteBuffer, std::monostate>;

    template <typename T, typename = void>
    struct PrimitiveReducer {
        typedef T type;
    };

    template <typename T>
    struct PrimitiveReducer<T, std::enable_if_t<std::is_convertible_v<T, SerializableUnitBase *>>> {
        typedef SerializableUnitBase *type;
    };

    template <typename T>
    struct PrimitiveReducer<T, std::enable_if_t<std::is_enum_v<T>>> {
        typedef int type;
    };

    template <typename T, typename = void>
    struct PrimitiveTypeIndex : type_pack_index<SerializePrimitives, typename PrimitiveReducer<T>::type> {
    };

    template <typename T>
    const constexpr size_t PrimitiveTypeIndex_v = PrimitiveTypeIndex<T>::value;

    template <typename T, typename = void>
    struct PrimitiveTypesContain : type_pack_contains<SerializePrimitives, typename PrimitiveReducer<T>::type> {
    };

    template <typename T>
    const constexpr bool PrimitiveTypesContain_v = PrimitiveTypesContain<T>::value;

    template <typename T>
    const constexpr bool isPrimitiveType_v = PrimitiveTypesContain_v<T> || std::is_convertible_v<T, SerializableUnitBase*> || std::is_enum_v<T>;

}
}
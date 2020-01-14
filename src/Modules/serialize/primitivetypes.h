#pragma once


#include "../generic/templates.h"

namespace Engine {
namespace Serialize {

	using SerializePrimitives = type_pack<bool, size_t, int, float, SerializableUnitBase *, std::string, Filesystem::Path, Vector2, Vector3, Matrix3, InvScopePtr>;

    template <typename T, typename = void>
    struct SerializableUnitReducer {
        typedef T type;
    };

    template <typename T>
    struct SerializableUnitReducer<T, std::enable_if_t<std::is_convertible_v<T, SerializableUnitBase *>>> {
        typedef SerializableUnitBase *type;
    };

    template <typename T, typename = void>
    struct PrimitiveTypeIndex : type_pack_index<SerializePrimitives, typename SerializableUnitReducer<T>::type> {
    };

    template <typename T>
    const constexpr size_t PrimitiveTypeIndex_v = PrimitiveTypeIndex<T>::value;

    template <typename T, typename = void>
    struct PrimitiveTypesContain : type_pack_contains<SerializePrimitives, T> {
    };

    template <typename T>
    const constexpr bool PrimitiveTypesContain_v = PrimitiveTypesContain<T>::value || std::is_convertible_v<T, SerializableUnitBase *>;

    template <typename T>
    const constexpr bool isPrimitiveType_v = PrimitiveTypesContain_v<T> || std::is_convertible_v<T, SerializableUnitBase*> || std::is_enum_v<T>;

}
}
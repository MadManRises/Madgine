#pragma once

#include "typedscopeptr.h"

namespace Engine {

using ValueTypeList = type_pack<
#define VALUETYPE_SEP ,
#define VALUETYPE_TYPE(Type, Storage, Name) Type
#include "valuetypedef.h"
    >;

enum class Type : unsigned char {
#define VALUETYPE_SEP ,
#define VALUETYPE_TYPE(Type, Storage, Name) Name##Value
#include "valuetypedef.h"
    ,
    MAX_VALUETYPE_TYPE
};

template <typename T>
using isValueTypePrimitive = type_pack_contains<ValueTypeList, T>;

template <typename T>
static constexpr bool isValueTypePrimitive_v = isValueTypePrimitive<T>::value;

template <typename Base, typename Derived>
struct ValueType_is_base_of : std::is_base_of<Base, Derived> {
};

template <typename Base, typename T>
struct ValueType_is_base_of<Base, VirtualIterator<T>> : std::false_type {
};

template <typename Base>
struct ValueType_is_base_of<Base, ValueType> : std::false_type {
};

template <typename Base, typename Derived>
static constexpr bool ValueType_is_base_of_v = ValueType_is_base_of<Base, Derived>::value;

template <typename T>
using isScopeRef = std::bool_constant<
    (std::is_pointer_v<T> && ValueType_is_base_of_v<ScopeBase, std::remove_pointer_t<T>> && !std::is_same_v<ScopeBase, std::remove_pointer_t<T>>)
    || (std::is_reference_v<T> && ValueType_is_base_of_v<ScopeBase, std::remove_pointer_t<std::remove_reference_t<T>>> && !std::is_same_v<ScopeBase, std::remove_pointer_t<std::remove_reference_t<T>>>)>;

template <typename T>
static constexpr bool isScopeRef_v = isScopeRef<T>::value;

template <typename T>
using isValueType = std::bool_constant<
    isValueTypePrimitive_v<std::decay_t<T>> || std::is_enum_v<std::decay_t<T>> || isScopeRef_v<T>>;

template <typename T>
static constexpr bool isValueType_v = isValueType<T>::value;

template <typename T>
using ValueType_Return = std::enable_if_t<isValueType_v<T>,
    std::conditional_t<
        isValueTypePrimitive_v<T> && !std::is_same_v<T, std::string_view>,
        const T &,
        T>>;

template <typename T, typename = std::enable_if_t<isValueType_v<T>>>
MODULES_EXPORT ValueType_Return<T> ValueType_as_impl(const ValueType &v);

template <typename T, typename = std::enable_if_t<isValueType_v<T> || std::is_same_v<T, ValueType>>>
decltype(auto) ValueType_as(const ValueType &v)
{
    if constexpr (std::is_same_v<T, ValueType>) {
        return v;
    } else if constexpr(isScopeRef_v<T>) {
        return ValueType_as_impl<TypedScopePtr>(v).safe_cast<std::remove_pointer_t<T>>();
    } else {
        return ValueType_as_impl<T>(v);
    }
}

template <typename T, typename = std::enable_if_t<isValueType_v<T>>>
decltype(auto) convert_ValueType(T &&t)
{
    if constexpr (isValueTypePrimitive_v<std::decay_t<T>>) {
        return std::forward<T>(t);
    } else if constexpr (std::is_enum_v<std::decay_t<T>>) {
        if constexpr (std::is_reference_v<T>) {
            return static_cast<int &>(t);
        } else {
            return static_cast<int>(t);
        }
    } else if constexpr (isScopeRef_v<T>) {
        if constexpr (std::is_pointer_v<std::decay_t<T>>) {
            return TypedScopePtr { t };
        } else {
            return TypedScopePtr { &t };
        }
    } else {
        static_assert(dependent_bool<T, false>::value);
    }
}

template <typename T, typename = std::enable_if_t<isValueTypePrimitive_v<std::decay_t<T>>>>
MODULES_EXPORT void to_ValueType_impl(ValueType &v, T &&t);

template <typename T, typename = std::enable_if_t<isValueTypePrimitive_v<std::decay_t<T>>>>
MODULES_EXPORT void to_ValueTypeRef_impl(ValueTypeRef &v, T &&t);

template <typename T, typename = std::enable_if_t<isValueType_v<T>>>
void to_ValueType(ValueType &v, T &&t)
{
    to_ValueType_impl(v, convert_ValueType(std::forward<T>(t)));
}

template <typename T, typename = std::enable_if_t<isValueType_v<T>>>
void to_ValueTypeRef(ValueTypeRef &v, T &&t)
{
    to_ValueTypeRef_impl(v, convert_ValueType(std::forward<T>(t)));
}

MODULES_EXPORT ValueType &KeyValuePair_key(KeyValuePair &p);
MODULES_EXPORT ValueTypeRef &KeyValuePair_value(KeyValuePair &p);

template <typename T>
void to_KeyValuePair(KeyValuePair &p, T &&t)
{
    to_ValueType(KeyValuePair_key(p), kvKey(t));
    to_ValueTypeRef(KeyValuePair_value(p), kvValue(std::forward<T>(t)));
}

MODULES_EXPORT const ValueType &getArgument(const ArgumentList &args, size_t index);

}
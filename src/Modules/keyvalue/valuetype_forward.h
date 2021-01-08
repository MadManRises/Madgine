#pragma once

#include "typedscopeptr.h"

#include "valuetype_types.h"

#include "../generic/functor.h"

#include "ownedscopeptr.h"

namespace Engine {

MODULES_EXPORT ValueType &KeyValuePair_key(KeyValuePair &p);
MODULES_EXPORT ValueTypeRef &KeyValuePair_value(KeyValuePair &p);

template <bool reference_to_ptr, typename T>
void to_ValueType(ValueType &v, T &&t);
template <bool reference_to_ptr, typename T>
void to_ValueTypeRef(ValueTypeRef &v, T &&t);

template <bool reference_to_ptr, typename T>
void to_KeyValuePair(KeyValuePair &p, T &&t)
{
    to_ValueType<reference_to_ptr>(KeyValuePair_key(p), kvKey(t));
    to_ValueTypeRef<reference_to_ptr>(KeyValuePair_value(p), kvValue(std::forward<T>(t)));
}

MODULES_EXPORT const ValueType &getArgument(const ArgumentList &args, size_t index);

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
    || (!std::is_pointer_v<T> && ValueType_is_base_of_v<ScopeBase, std::decay_t<T>> && !std::is_same_v<ScopeBase, std::decay_t<T>>)
    || has_function_customScopePtr_v<T>>;

template <typename T>
static constexpr bool isScopeRef_v = isScopeRef<T>::value;

template <typename T>
using isValueType = std::bool_constant<
    isValueTypePrimitive_v<std::decay_t<T>> || std::is_enum_v<std::decay_t<T>> || isScopeRef_v<T> || is_iterable_v<T>>;

template <typename T>
static constexpr bool isValueType_v = isValueType<T>::value;

template <typename T>
using ValueTypePrimitiveSubList = type_pack_select_t<type_pack_index_v<size_t, ValueTypeList, T>, ValueTypeList>;

template <typename T>
using QualifiedValueTypePrimitiveSubList = type_pack_select_t<type_pack_index_v<size_t, ValueTypeList, T>, QualifiedValueTypeList>;

template <typename T, typename = void>
struct ValueType_ReturnHelper {
    typedef type_pack_select_t<type_pack_index_v<size_t, ValueTypePrimitiveSubList<T>, T>, QualifiedValueTypePrimitiveSubList<T>> type;
};

template <typename T>
struct ValueType_ReturnHelper<T, std::enable_if_t<std::is_base_of_v<ScopeBase, T>>> {
    typedef T &type;
};

template <typename T>
struct ValueType_ReturnHelper<T *, void> {
    typedef T *type;
};

template <typename T>
using ValueType_Return = typename ValueType_ReturnHelper<T>::type;

template <typename T, typename = std::enable_if_t<isValueTypePrimitive_v<T>>>
MODULES_EXPORT ValueType_Return<T> ValueType_as_impl(const ValueType &v);

template <typename T>
decltype(auto) ValueType_as(const ValueType &v)
{
    if constexpr (std::is_same_v<T, ValueType>) {
        return v;
    } else if constexpr (isScopeRef_v<T>) {
        if constexpr (std::is_pointer_v<T>) {
            return ValueType_as_impl<TypedScopePtr>(v).safe_cast<std::remove_pointer_t<T>>();
        } else {
            return ValueType_as_impl<OwnedScopePtr>(v).safe_cast<T>();
        }
    } else if constexpr (isValueTypePrimitive_v<T>) {
        return ValueType_as_impl<T>(v);
    } else if constexpr (is_iterable_v<T>) {
        return ValueType_as_impl<KeyValueVirtualRange>(v).safe_cast<T>();
    } else {
        static_assert(dependent_bool<T, false>::value, "A ValueType can not be converted to the given target type");
    }
}

template <bool reference_to_ptr, typename T>
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
    } else if constexpr (isScopeRef_v<std::decay_t<T>>) {
        if constexpr (std::is_pointer_v<std::decay_t<T>>) {
            return TypedScopePtr { t };
        } else if constexpr (std::is_reference_v<T> && reference_to_ptr) {
            return TypedScopePtr { &t };
        } else {
            return OwnedScopePtr { std::forward<T>(t) };
        }
    } else if constexpr (is_iterable_v<T>) {
        return KeyValueVirtualRange { std::forward<T>(t), type_holder<Functor<to_KeyValuePair<true, decltype(*std::declval<typename derive_iterator<T>::iterator>())>>> };
    } else if constexpr (TupleUnpacker::is_tuplefyable_v<std::remove_reference_t<T>>) {
        throw "TODO";
        return 3;
    } else {
        static_assert(dependent_bool<T, false>::value, "The provided type can not be converted to a ValueType");
    }
}

template <typename T, typename = std::enable_if_t<isValueTypePrimitive_v<std::decay_t<T>>>>
MODULES_EXPORT void to_ValueType_impl(ValueType &v, T &&t);

template <typename T, typename = std::enable_if_t<isValueTypePrimitive_v<std::decay_t<T>>>>
MODULES_EXPORT void to_ValueTypeRef_impl(ValueTypeRef &v, T &&t);

template <bool reference_to_ptr, typename T>
void to_ValueType(ValueType &v, T &&t)
{
    to_ValueType_impl(v, convert_ValueType<reference_to_ptr>(std::forward<T>(t)));
}

template <bool reference_to_ptr, typename T>
void to_ValueTypeRef(ValueTypeRef &v, T &&t)
{
    to_ValueTypeRef_impl(v, convert_ValueType<reference_to_ptr>(std::forward<T>(t)));
}

}
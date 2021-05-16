#pragma once

#include "typedscopeptr.h"

#include "Generic/functor.h"

#include "ownedscopeptr.h"

#include "Generic/container/virtualrange.h"

#include "Generic/keyvalue.h"

#include "valuetype_desc.h"

#include "enumholder.h"

namespace Engine {

META_EXPORT ValueType &KeyValuePair_key(KeyValuePair &p);
META_EXPORT ValueTypeRef &KeyValuePair_value(KeyValuePair &p);

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

template <bool reference_to_ptr>
struct Functor_to_KeyValuePair {
    template <typename... Args>
    decltype(auto) operator()(Args &&... args)
    {
        return to_KeyValuePair<reference_to_ptr>(std::forward<Args>(args)...);
    }
};

template <bool reference_to_ptr>
struct Functor_to_ValueTypeRef {
    template <typename... Args>
    decltype(auto) operator()(Args &&... args)
    {
        return to_ValueTypeRef<reference_to_ptr>(std::forward<Args>(args)...);
    }
};

META_EXPORT const ValueType &getArgument(const ArgumentList &args, size_t index);

template <typename T>
using ValueTypePrimitiveSubList = type_pack_select_t<type_pack_index_v<size_t, ValueTypeList, T>, ValueTypeList>;

template <typename T>
using QualifiedValueTypePrimitiveSubList = type_pack_select_t<type_pack_index_v<size_t, ValueTypeList, T>, QualifiedValueTypeList>;

template <typename T, typename = void>
struct ValueType_ReturnHelper {
    typedef T &type;
};

template <typename T>
struct ValueType_ReturnHelper<T, std::enable_if_t<isValueTypePrimitive_v<T>>> {
    typedef type_pack_select_t<type_pack_index_v<size_t, ValueTypePrimitiveSubList<T>, T>, QualifiedValueTypePrimitiveSubList<T>> type;
};

template <typename T>
struct ValueType_ReturnHelper<T *, void> {
    typedef T *type;
};

template <typename T>
using ValueType_Return = typename ValueType_ReturnHelper<T>::type;

template <typename T, typename = std::enable_if_t<isValueTypePrimitive_v<T>>>
META_EXPORT ValueType_Return<T> ValueType_as_impl(const ValueType &v);

template <typename T>
decltype(auto) ValueType_as(const ValueType &v)
{
    if constexpr (std::is_same_v<T, ValueType>) {
        return v;
    } else if constexpr (isValueTypePrimitive_v<T>) {
        return ValueType_as_impl<T>(v);
    } else if constexpr (is_iterable_v<T>) {
        if constexpr (std::is_same_v<KeyType_t<typename T::iterator::value_type>, std::monostate>)
            return ValueType_as_impl<KeyValueVirtualSequenceRange>(v).safe_cast<T>();
        else
            return ValueType_as_impl<KeyValueVirtualAssociativeRange>(v).safe_cast<T>();
    } else if constexpr (is_instance_v<T, Enum> || is_instance_v<T, BaseEnum>) {
        return ValueType_as_impl<EnumHolder>(v).safe_cast<T>();
    } else {
        if constexpr (std::is_pointer_v<T>) {
            return ValueType_as_impl<TypedScopePtr>(v).safe_cast<std::remove_pointer_t<T>>();
        } else {
            return ValueType_as_impl<OwnedScopePtr>(v).safe_cast<T>();
        }
    }
    //static_assert(dependent_bool<T, false>::value, "A ValueType can not be converted to the given target type");
}

META_EXPORT ValueTypeDesc ValueType_type(const ValueType &v);

template <typename T>
bool ValueType_is(const ValueType &v)
{
    return toValueTypeDesc<T>().canAccept(ValueType_type(v));
}

template <bool reference_to_ptr, typename T>
decltype(auto) convert_ValueType(T &&t)
{
    if constexpr (isValueTypePrimitive_v<std::decay_t<T>> || std::is_same_v<ValueType, std::decay_t<T>>) {
        return std::forward<T>(t);
    } else if constexpr (is_string_like_v<std::decay_t<T>>) {
        return std::string { std::forward<T>(t) };
    } else if constexpr (is_iterable_v<T>) {
        if constexpr (std::is_same_v<KeyType_t<typename std::remove_reference_t<T>::iterator::value_type>, std::monostate>)
            return KeyValueVirtualSequenceRange { std::forward<T>(t) };
        else
            return KeyValueVirtualAssociativeRange { std::forward<T>(t) };
    } else if constexpr (std::is_enum_v<std::decay_t<T>>) {
        if constexpr (std::is_reference_v<T>) {
            return static_cast<std::underlying_type_t<T> &>(t);
        } else {
            return static_cast<std::underlying_type_t<T>>(t);
        }
    } else if constexpr (is_instance_v<T, Enum> || is_instance_v<T, BaseEnum>) {
        return EnumHolder { std::forward<T>(t) };
    } else {
        if constexpr (std::is_pointer_v<std::decay_t<T>>) {
            return TypedScopePtr { t };
        } else if constexpr (is_instance_v<std::decay_t<T>, std::unique_ptr>) {
            return TypedScopePtr { t.get() };
        } else if constexpr (std::is_reference_v<T> && reference_to_ptr) {
            return TypedScopePtr { &t };
        } else {
            return OwnedScopePtr { std::forward<T>(t) };
        }
    }
    //static_assert(dependent_bool<T, false>::value, "The provided type can not be converted to a ValueType");
}

template <typename T, typename = std::enable_if_t<isValueTypePrimitive_v<std::decay_t<T>> || std::is_same_v<ValueType, std::decay_t<T>>>>
META_EXPORT void to_ValueType_impl(ValueType &v, T &&t);

template <typename T, typename = std::enable_if_t<isValueTypePrimitive_v<std::decay_t<T>> || std::is_same_v<ValueType, std::decay_t<T>>>>
META_EXPORT void to_ValueTypeRef_impl(ValueTypeRef &v, T &&t);

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
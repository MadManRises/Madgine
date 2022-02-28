#pragma once

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
    decltype(auto) operator()(Args &&...args)
    {
        return to_KeyValuePair<reference_to_ptr>(std::forward<Args>(args)...);
    }
};

template <bool reference_to_ptr>
struct Functor_to_ValueTypeRef {
    template <typename... Args>
    decltype(auto) operator()(Args &&...args)
    {
        return to_ValueTypeRef<reference_to_ptr>(std::forward<Args>(args)...);
    }
};

META_EXPORT const ValueType &getArgument(const ArgumentList &args, size_t index);

template <typename T>
using ValueTypePrimitiveSubList = ValueTypeList::select<type_pack_index_v<size_t, ValueTypeList, T>>;

template <typename T>
using QualifiedValueTypePrimitiveSubList = QualifiedValueTypeList::select<type_pack_index_v<size_t, ValueTypeList, T>>;

template <typename T>
struct ValueType_ReturnHelper {
    typedef T &type;
};

template <ValueTypePrimitive T>
struct ValueType_ReturnHelper<T> {
    typedef typename QualifiedValueTypePrimitiveSubList<T>::template select<type_pack_index_v<size_t, ValueTypePrimitiveSubList<T>, T>> type;
};

template <typename T>
struct ValueType_ReturnHelper<T *> {
    typedef T *type;
};

template <typename T>
using ValueType_Return = typename ValueType_ReturnHelper<T>::type;

META_EXPORT bool ValueType_isNull(const ValueType &v);
META_EXPORT ValueTypeDesc ValueType_type(const ValueType &v);

template <typename T>
bool ValueType_is(const ValueType &v)
{
    return toValueTypeDesc<T>().canAccept(ValueType_type(v));
}

template <ValueTypePrimitive T>
META_EXPORT ValueType_Return<T> ValueType_as_impl(const ValueType &v);

template <typename T>
decltype(auto) ValueType_as(const ValueType &v)
{
    if constexpr (InstanceOf<T, std::optional>) {
        if (ValueType_isNull(v))
            return T {};
        else {
            return T { ValueType_as<type_pack_unpack_unique_t<typename is_instance<T, std::optional>::argument_types>>(v) };
        }
    } else if constexpr (std::same_as<T, ValueType>) {
        return v;
    } else if constexpr (ValueTypePrimitive<T>) {
        return ValueType_as_impl<T>(v);
    } else if constexpr (std::ranges::range<T>) {
        if constexpr (std::is_same_v<KeyType_t<typename T::iterator::value_type>, std::monostate>)
            return ValueType_as_impl<KeyValueVirtualSequenceRange>(v).safe_cast<T>();
        else
            return ValueType_as_impl<KeyValueVirtualAssociativeRange>(v).safe_cast<T>();
    } else if constexpr (InstanceOf<T, EnumType> || InstanceOf<T, BaseEnum>) {
        return ValueType_as_impl<EnumHolder>(v).safe_cast<T>();
    } else {
        if constexpr (Pointer<T>) {
            return ValueType_as_impl<TypedScopePtr>(v).safe_cast<std::remove_pointer_t<T>>();
        } else {
            return ValueType_as_impl<OwnedScopePtr>(v).safe_cast<T>();
        }
    }
    //static_assert(dependent_bool<T, false>::value, "A ValueType can not be converted to the given target type");
}

template <bool reference_to_ptr, typename T>
decltype(auto) convert_ValueType(T &&t)
{
    if constexpr (InstanceOf<T, std::optional>) {
        return std::forward<T>(t);
    } else if constexpr (ValueTypePrimitive<std::decay_t<T>> || std::is_same_v<ValueType, std::decay_t<T>>) {
        return std::forward<T>(t);
    } else if constexpr (String<std::decay_t<T>>) {
        return std::string { std::forward<T>(t) };
    } else if constexpr (std::ranges::range<T>) {
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
    } else if constexpr (InstanceOf<T, EnumType> || InstanceOf<T, BaseEnum>) {
        return EnumHolder { std::forward<T>(t) };
    } else {
        if constexpr (Pointer<std::decay_t<T>>) {
            return TypedScopePtr { t };
        } else if constexpr (InstanceOf<std::decay_t<T>, std::unique_ptr>) {
            return TypedScopePtr { t.get() };
        } else if constexpr (std::is_reference_v<T> && reference_to_ptr) {
            return TypedScopePtr { &t };
        } else {
            return OwnedScopePtr { std::forward<T>(t) };
        }
    }
    //static_assert(dependent_bool<T, false>::value, "The provided type can not be converted to a ValueType");
}

template <typename T>
requires(ValueTypePrimitive<std::decay_t<T>> || std::same_as<ValueType, std::decay_t<T>>)
    META_EXPORT void to_ValueType_impl(ValueType &v, T &&t);

template <typename T>
requires(ValueTypePrimitive<std::decay_t<T>> || std::same_as<ValueType, std::decay_t<T>>)
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
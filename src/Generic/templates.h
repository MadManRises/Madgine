#pragma once

namespace Engine {
template <typename T>
using identity = T;

template <typename V, typename...>
struct last {
    typedef V type;
};

template <typename U, typename V, typename... T>
struct last<U, V, T...> : last<V, T...> {
};

template <typename V, typename...>
struct first {
    typedef V type;
};

template <typename... Args>
using first_t = typename first<Args...>::type;

template <typename...>
using void_t = void;

template <typename T, typename...>
using dependent_t = T;

template <typename T, bool b>
struct dependent_bool : std::bool_constant<b> {
};

template <typename... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template <typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template <typename T, size_t S>
constexpr size_t array_size(T (&)[S])
{
    return S;
}

template <template <typename> typename F, typename T, bool b>
struct transform_if {
    using type = T;
};

template <template <typename> typename F, typename T>
struct transform_if<F, T, true> {
    using type = F<T>;
};

template <template <typename> typename F, typename T, bool b>
using transform_if_t = typename transform_if<F, T, b>::type;

template <typename, template <typename...> typename>
struct is_instance : std::false_type {
};

template <typename... T, template <typename...> typename U>
struct is_instance<U<T...>, U> : std::true_type {
    using argument_types = type_pack<T...>;
};

template <size_t add, typename Sequence>
struct index_range_add;

template <size_t add, size_t... Is>
struct index_range_add<add, std::index_sequence<Is...>> {
    using type = std::index_sequence<(Is + add)...>;
};

template <size_t from, size_t to>
using make_index_range = typename index_range_add<from, std::make_index_sequence<to - from>>::type;

template <typename Base, std::derived_from<Base> Derived>
constexpr size_t inheritance_offset()
{
    return reinterpret_cast<uintptr_t>(static_cast<Base *>(reinterpret_cast<Derived *>(0x1))) - 1;
}

template <typename T, template <typename...> typename U>
concept InstanceOf = is_instance<T, U>::value;

template <typename T>
concept Tuple = InstanceOf<T, std::tuple>;

template <typename T>
concept String = std::is_constructible_v<std::string, const T &> &&std::is_constructible_v<T, const std::string &>;

template <typename T>
concept StringViewable = std::is_constructible_v<std::string_view, const T &>;

template <typename T>
concept Enum = std::is_enum_v<T>;

template <typename T>
concept Pointer = std::is_pointer_v<T> || std::is_null_pointer_v<T>;

template <typename T>
concept Function = std::is_function_v<T>;

template <typename T>
concept Unsigned = std::is_unsigned_v<T>;

template <typename T, typename... Ty>
concept OneOf = (std::same_as<T, Ty> || ...);

template <typename T>
struct OutRef {

    OutRef() = default;

    OutRef(T &t)
        : mPtr(&t)
    {
    }

    OutRef &operator=(T &t)
    {
        mPtr = &t;
        return *this;
    }

    operator T &()
    {
        return *mPtr;
    }

    T *mPtr = nullptr;
};

template <typename F, typename... Args>
auto invoke_patch_void(F &&f, Args &&...args)
{
    if constexpr (std::same_as<std::invoke_result_t<F, Args...>, void>) {
        std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
        return std::monostate {};
    } else {
        return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    }
}

template <typename T>
using patch_void_t = std::conditional_t<std::same_as<T, void>, std::monostate, T>;

}

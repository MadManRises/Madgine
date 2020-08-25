#pragma once

namespace Engine {
struct CopyType {
};

struct NonCopyType {
    NonCopyType() = default;
    NonCopyType(const NonCopyType &) = delete;
    void operator=(const NonCopyType &) = delete;
    NonCopyType(NonCopyType &&) = default;
    NonCopyType &operator=(NonCopyType &&) = default;
};

namespace __generic__impl__ {
    template <typename, typename T>
    struct _custom_is_copy_constructible : std::false_type {
    };

    template <typename T>
    struct _custom_is_copy_constructible<std::void_t<decltype(T(std::declval<const T &>()))>, T> : std::true_type {
    };
}

template <typename T>
struct custom_is_copy_constructible {
    static const constexpr bool value = __generic__impl__::_custom_is_copy_constructible<std::void_t<>, T>::value;
};

template <typename U, typename V>
struct custom_is_copy_constructible<std::pair<U, V>> {
    static constexpr const bool value = custom_is_copy_constructible<U>::value && custom_is_copy_constructible<V>::value;
};

template <typename T>
using CopyTraits = std::conditional_t<custom_is_copy_constructible<T>::value, CopyType, NonCopyType>;

template <typename T, typename = std::enable_if_t<custom_is_copy_constructible<T>::value>>
T tryCopy(T &t)
{
    return t;
}

template <typename T, typename = std::enable_if_t<!custom_is_copy_constructible<std::remove_reference_t<T>>::value>>
std::remove_reference_t<T> tryCopy(T &&t)
{
    std::terminate();
}

}
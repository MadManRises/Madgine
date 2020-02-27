#pragma once

namespace Engine {

	
template <typename T>
struct FixString {
    static T apply(T &&t)
    {
        return std::forward<T>(t);
    }

    using type = T;
};

template <>
struct FixString<const char *> {
    static std::string apply(const char *s)
    {
        return s;
    }
    using type = std::string;
};

template <>
struct FixString<std::string_view> {
    static std::string apply(const std::string_view &s)
    {
        return std::string { s };
    }
    using type = std::string;
};

template <typename T>
using FixString_t = typename FixString<T>::type;


}
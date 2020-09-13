#pragma once

namespace Engine {

	
template <typename T>
struct MakeOwning {
    static T apply(T &&t)
    {
        return std::forward<T>(t);
    }

    using type = T;
};

template <>
struct MakeOwning<const char *> {
    static std::string apply(const char *s)
    {
        return s;
    }
    using type = std::string;
};

template <>
struct MakeOwning<std::string_view> {
    static std::string apply(const std::string_view &s)
    {
        return std::string { s };
    }
    using type = std::string;
};

template <typename T>
using MakeOwning_t = typename MakeOwning<T>::type;


}
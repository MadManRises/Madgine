#pragma once

namespace Engine {
		
template <typename, template <typename...> typename>
struct is_instance : std::false_type {
};

template <typename... T, template <typename...> typename U>
struct is_instance<U<T...>, U> : std::true_type {
    using argument_types = type_pack<T...>;
};

template <typename T, template <typename...> typename U>
concept InstanceOf = is_instance<T, U>::value;

template <typename T>
concept Tuple = InstanceOf<T, std::tuple>;

template <typename T>
concept String = std::is_constructible_v<std::string, const T &> && std::is_constructible_v<T, const std::string &>;

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

template <typename T, typename... Ty>
concept NoneOf = !OneOf<T, Ty...>;

template <typename T, typename... Ty>
concept DecayedNoneOf = NoneOf<std::decay_t<T>, Ty...>;

template <typename T>
concept Reference = std::is_reference_v<T> || InstanceOf<T, std::reference_wrapper>;

}
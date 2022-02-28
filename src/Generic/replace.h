#pragma once

#include "type_pack.h"

namespace Engine {

	
struct DefaultTag;

template <typename Tag, size_t>
struct TaggedPlaceholder;

template <size_t N>
using Placeholder = TaggedPlaceholder<DefaultTag, N>;

template <typename T>
struct replace {
    template <typename Tag, typename...>
    using tagged = T;

    template <typename...>
    using type = T;
};

template <typename Tag, size_t N>
struct replace<TaggedPlaceholder<Tag, N>> {
    template <typename Tag2, typename... Args>
    using tagged = std::conditional_t<std::is_same_v<Tag, Tag2>, typename type_pack<Args...>::template select<N>, TaggedPlaceholder<Tag, N>>;

    template <typename... Args>
    using type = tagged<DefaultTag, Args...>;
};

template <template <typename...> typename C, typename... Ty>
struct replace<C<Ty...>> {
    template <typename Tag, typename... Args>
    using tagged = C<typename replace<Ty>::template tagged<Tag, Args...>...>;

    template <typename... Args>
    using type = tagged<DefaultTag, Args...>;
};

template <template <auto, typename...> typename C, auto V, typename... Ty>
struct replace<C<V, Ty...>> {
    template <typename Tag, typename... Args>
    using tagged = C<V, typename replace<Ty>::template tagged<Tag, Args...>...>;

    template <typename... Args>
    using type = tagged<DefaultTag, Args...>;
};

template <template <template <typename...> typename, typename...> typename C, template<typename...> typename V, typename... Ty>
struct replace<C<V, Ty...>> {
    template <typename Tag, typename... Args>
    using tagged = C<V, typename replace<Ty>::template tagged<Tag, Args...>...>;

    template <typename... Args>
    using type = tagged<DefaultTag, Args...>;
};

}
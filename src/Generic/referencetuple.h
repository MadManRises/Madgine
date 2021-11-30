#pragma once

namespace Engine {

template <typename... Ty>
struct ReferenceTuple {

    ReferenceTuple(std::tuple<Ty &...> data)
        : mData(data)
    {
    }

    operator std::tuple<Ty &...>()
    {
        return mData;
    }

    template <size_t I>
    friend decltype(auto) get(ReferenceTuple &data)
    {
        return std::get<I>(data.mData);
    }

    template <size_t I>
    friend decltype(auto) get(const ReferenceTuple &data)
    {
        return std::get<I>(data.mData);
    }

    template <typename T>
    friend decltype(auto) get(ReferenceTuple &data)
    {
        return std::get<T&>(data.mData);
    }

    template <typename T>
    friend decltype(auto) get(const ReferenceTuple &data)
    {
        return std::get<T>(data.mData);
    }

    std::tuple<Ty *...> operator&() const
    {
        return {
            { &std::get<Ty &>(mData)... }
        };
    }

    friend void swap(const ReferenceTuple &first, const ReferenceTuple &second)
    {
        (std::swap(std::get<Ty &>(first.mData), std::get<Ty &>(second.mData)), ...);
    }

private:
    std::tuple<Ty &...> mData;
};

}

namespace std {

template <typename... Ty>
struct tuple_size<Engine::ReferenceTuple<Ty...>> : std::integral_constant<size_t, sizeof...(Ty)> {
};


    template <size_t I, typename... Ty>
struct tuple_element<I, Engine::ReferenceTuple<Ty...>> {
    using type = Engine::type_pack_select_t<I, Engine::type_pack<Ty...>>&;
};

}
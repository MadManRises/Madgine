#pragma once

#include "type_pack.h"

namespace Engine {

template <typename F, typename Pack>
struct BuilderImpl {

    using Tuple = type_pack_as_tuple_t<type_pack_apply_t<type_pack_as_tuple_t, Pack>>;

    BuilderImpl(F &&f)
        : mF(std::move(f))
    {
    }

    BuilderImpl(std::optional<F> f = {}, Tuple data = {})
        : mF(std::move(f))
        , mData(std::move(data))
    {
    }

    BuilderImpl(BuilderImpl<F, Pack> &&other)
        : mF(std::move(other.mF))
        , mData(std::move(other.mData))
    {
        other.mF.reset();
    }

    ~BuilderImpl() 
    {
        if (mF) {
            execute();
        }
    }

    auto execute() {
        assert(mF);
        F f = *std::move(mF);
        mF.reset();
        return TupleUnpacker::invokeFromTuple(std::move(f), std::move(mData));
    }

    operator decltype(TupleUnpacker::invokeFromTuple(std::declval<F&&>(), std::declval<Tuple&&>()))()
    {
        return execute();
    }

    template <typename T>
    struct appender {
        template <typename Pack>
        using type = type_pack_append_t<Pack, T>;
    };

    template <size_t Dim = 0, typename T = void>
    auto append(T &&t)
    {
        static_assert(Dim < type_pack_size_v<Pack>);
        return append_impl<Dim>(std::forward<T>(t), type_pack_indices_t<Pack> {});
    }

private:
    template <size_t Dim, typename T = void, size_t... Is>
    BuilderImpl<F, type_pack_apply_to_nth_t<appender<T>::template type, Pack, Dim>> append_impl(T &&t, std::index_sequence<Is...>)
    {
        std::optional<F> f = std::move(mF);
        mF.reset();
        return {
            std::move(f),
            std::forward_as_tuple(
                [&]() {
                    if constexpr (Is == Dim)
                        return std::tuple_cat(std::get<Is>(std::move(mData)), std::forward_as_tuple(std::forward<T>(t)));
                    else
                        return std::get<Is>(std::move(mData));
                }()...)
        };
        
    }

private:
    std::optional<F> mF;
    Tuple mData;
};

template <typename F, size_t Dim = 1>
using Builder = BuilderImpl<F, type_pack_repeat_n_times<type_pack<>, Dim>>;

}
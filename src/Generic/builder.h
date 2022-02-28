#pragma once

#include "type_pack.h"
#include "then.h"

namespace Engine {

template <typename F, typename Pack, template <typename Builder> typename Facade>
struct BuilderImpl {

    using Tuple = typename Pack::transform<type_pack_as_tuple>::as_tuple;
    using return_type = decltype(TupleUnpacker::invokeFromTuple(std::declval<F &&>(), std::declval<Tuple &&>()));

    BuilderImpl(F &&f)
        : mF(std::move(f))
    {
    }

    BuilderImpl(F f, Tuple data)
        : mF(std::move(f))
        , mData(std::move(data))
    {
    }

    BuilderImpl(const BuilderImpl<F, Pack, Facade> &) = delete;
    BuilderImpl(BuilderImpl<F, Pack, Facade> &&other) = default;

    ~BuilderImpl()
    {
        if (mF) {
            execute();
        }
    }

    decltype(auto) execute()
    {
        F f = std::move(*mF);
        mF.reset();
        return TupleUnpacker::invokeFromTuple(std::move(f), std::move(mData));
    }

    operator return_type()
    {
        return execute();
    }

    template <typename T>
    requires std::convertible_to<return_type, T>
    operator T()
    {
        return execute();
    }

    template <typename G>
    auto then(G &&g) &&
    {
        auto modified_f = [f { std::move(mF) }, g { std::forward<G>(g) }](auto &&...args) mutable -> decltype(auto) {
            return Engine::then(std::move(f)(std::forward<decltype(args)>(args)...), std::move(g));
        };
        return Facade<BuilderImpl<decltype(modified_f), Pack, Facade>> {
            std::move(modified_f),
            std::move(mData)
        };
    }

protected:
    template <size_t Dim, typename T>
    auto append(T &&t) &&
    {
        static_assert(Dim < Pack::size);
        return append_impl<Dim, T>(std::forward<T>(t), typename Pack::indices {});
    }

private:
    
    template <size_t Dim, typename T, size_t... Is>
    Facade<BuilderImpl<F, typename Pack::transform_nth<type_pack_appender<T>::template type, Dim>, Facade>> append_impl(T &&t, std::index_sequence<Is...>)
    {
        F f = std::move(*mF);
        mF.reset();
        return { std::move(f),
            std::forward_as_tuple(
                [&]() {
                    if constexpr (Is == Dim)
                        return std::tuple_cat(std::get<Is>(std::move(mData)), std::forward_as_tuple(std::forward<T>(t)));
                    else
                        return std::get<Is>(std::move(mData));
                }()...) };
    }

private:
    std::optional<F> mF;
    Tuple mData;
};

template <typename F, template <typename Builder> typename Facade, size_t Dim = 1>
using Builder = BuilderImpl<F, type_pack_repeat_n_times<type_pack<>, Dim>, Facade>;

}
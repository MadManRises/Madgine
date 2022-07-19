#pragma once

#if __cpp_lib_ranges < 202110L

namespace std {
namespace ranges {

#if (__cpp_lib_ranges < 201911L) || ANDROID || OSX || IOS
    template <class T>
    concept range = requires(T &t)
    {
        t.begin(); // equality-preserving for forward iterators
        t.end();
    };
#endif


    template <typename C, typename F>
    constexpr auto find_if_not(C &&c, F &&f)
    {
        return std::find_if_not(std::forward<C>(c).begin(), std::forward<C>(c).end(), std::forward<F>(f));
    }

    template <typename C, typename F>
    constexpr auto find_if(C &&c, F &&f)
    {
        return std::find_if(std::forward<C>(c).begin(), std::forward<C>(c).end(), std::forward<F>(f));
    }

    template <typename C, typename O, typename F>
    constexpr auto transform(C &&c, O o, F &&op)
    {
        return std::transform(std::forward<C>(c).begin(), std::forward<C>(c).end(), o, std::forward<F>(op));
    }

    template <typename C, typename O>
    constexpr auto copy(C &&c, O o)
    {
        return std::copy(std::forward<C>(c).begin(), std::forward<C>(c).end(), o);
    }

    template <typename C, typename I, typename P>
    constexpr auto find(C &&c, I &&i, P &&p)
    {
        return std::find_if(std::forward<C>(c).begin(), std::forward<C>(c).end(), [&](auto &&e) {
            return std::invoke(p, std::forward<decltype(e)>(e)) == i;
        });
    }

    template <typename C, typename I>
    constexpr auto find(C &&c, I &&i)
    {
        return std::find(std::forward<C>(c).begin(), std::forward<C>(c).end(), std::forward<I>(i));
    }

    template <typename C, typename Cmp>
    constexpr auto sort(C &&c, Cmp &&cmp)
    {
        return std::sort(std::forward<C>(c).begin(), std::forward<C>(c).end(), std::forward<Cmp>(cmp));
    }

}
}

#endif

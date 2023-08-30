#pragma once

#include "../pipable.h"

#if __cpp_lib_ranges >= 201911L
#    include <ranges>
#endif

namespace std {
namespace ranges {

#if (__cpp_lib_ranges < 201911L)
    template <class T>
    concept range = requires(T &t)
    {
        t.begin();
        t.end();
    };
#endif

#if __cpp_lib_ranges < 202110L
    template <typename C, typename F>
    constexpr auto find_if_not(C &&c, F &&f)
    {
        return std::find_if_not(std::forward<C>(c).begin(), std::forward<C>(c).end(), std::forward<F>(f));
    }

    template <typename C, typename F>
    constexpr auto find_if(C &&c, F &&f)
    {
        auto end = c.end();
        auto it = c.begin();
        for (; it != end; ++it) {
            if (f(*it)) {
                return it;
            }
        }
        return it;
    }

    template <typename C, typename F, typename P>
    constexpr auto find_if(C &&c, F &&f, P &&p)
    {
        auto end = c.end();
        auto it = c.begin();
        for (; it != end; ++it) {
            if (f(std::invoke(p, *it))) {
                return it;
            }
        }
        return it;
    }

    template <typename C, typename O, typename F>
    constexpr auto transform(C &&c, O o, F &&op)
    {
        for (auto &&e : std::forward<C>(c))
            *o++ = op(std::forward<decltype(e)>(e));
        return o;
    }

    template <typename C, typename O>
    constexpr auto copy(C &&c, O o)
    {
        return std::copy(std::forward<C>(c).begin(), std::forward<C>(c).end(), o);
    }

    template <typename C, typename O>
    constexpr auto move(C &&c, O o)
    {
        return std::move(std::forward<C>(c).begin(), std::forward<C>(c).end(), o);
    }

    template <typename C, typename I, typename P>
    constexpr auto find(C &&c, I &&i, P &&p)
    {
        return std::ranges::find_if(std::forward<C>(c), [&](auto &&e) {
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

    template <typename C, typename T>
    constexpr auto lower_bound(C &&c, T &&t)
    {
        return std::lower_bound(std::forward<C>(c).begin(), std::forward<C>(c).end(), std::forward<T>(t));
    }

    template <typename C, typename F>
    constexpr bool all_of(C &&c, F &&f)
    {
        return std::all_of(std::forward<C>(c).begin(), std::forward<C>(c).end(),
            std::forward<F>(f));
    }

    template <typename T>
    using range_value_t = typename std::remove_reference_t<T>::value_type;
#endif

#if __cpp_lib_ranges < 202207L
    template<range R>
    using const_iterator_t = decltype(cbegin(std::declval<R&>()));
#endif
}

#if __cpp_lib_ranges < 202110L
namespace views {
    template <typename R>
    struct all_t {

        all_t(R &&r)
            : mR(std::forward<R>(r))
        {
        }

        using iterator = decltype(std::declval<R>().begin());

        iterator begin()
        {
            return mR.begin();
        }

        iterator end()
        {
            return mR.end();
        }

        size_t size() const
        {
            return mR.size();
        }

        R mR;
    };

    template <typename R, typename F>
    struct TransformView {

        struct iterator {
            using It = decltype(std::declval<R>().begin());
            using difference_type = typename It::difference_type;
            using value_type = std::invoke_result_t<F, decltype(*std::declval<It>())>;

            decltype(auto) operator*() const
            {
                return (*mF)(*mIt);
            }

            bool operator==(const iterator &other) const
            {
                return mIt == other.mIt;
            }

            bool operator!=(const iterator &other) const
            {
                return mIt != other.mIt;
            }

            iterator &operator++()
            {
                ++mIt;
                return *this;
            }

            iterator operator++(int)
            {
                iterator copy = *this;
                ++(*this);
                return copy;
            }

            It mIt;
            std::remove_reference_t<F> *mF;
        };

        using value_type = typename iterator::value_type;

        iterator begin()
        {
            return { mR.begin(), &mF };
        }

        iterator end()
        {
            return { mR.end(), &mF };
        }

        size_t size() const
        {
            return mR.size();
        }

        bool empty() const
        {
            return mR.empty();
        }

        R mR;
        F mF;
    };

    template <typename R, typename F>
    TransformView<R, F> transform(R &&r, F &&f)
    {
        return { std::forward<R>(r), std::forward<F>(f) };
    }

    MAKE_PIPABLE_FROM_RIGHT(transform)

    template <typename R>
    struct ReverseView {

        using iterator = std::reverse_iterator<decltype(std::declval<R>().begin())>;

        iterator begin()
        {
            return mR.rbegin();
        }

        iterator end()
        {
            return mR.rend();
        }

        R mR;
    };

    struct reverse_t {

        template <typename R>
        ReverseView<R> operator()(R &&r) const
        {
            return { std::forward<R>(r) };
        }

        template <typename R>
        friend ReverseView<R> operator|(R &&r, reverse_t)
        {
            return { std::forward<R>(r) };
        }
    };

    static constexpr reverse_t reverse;
}

namespace ranges {
    template <typename T>
    inline constexpr bool enable_borrowed_range = false;

    template <typename R>
    concept borrowed_range = range<R> &&(std::is_lvalue_reference_v<R> || enable_borrowed_range<std::remove_cvref_t<R>>);
}

template <typename R>
inline constexpr bool ranges::enable_borrowed_range<views::ReverseView<R>> = ranges::borrowed_range<R>;

template <typename R, typename F>
inline constexpr bool ranges::enable_borrowed_range<views::TransformView<R, F>> = ranges::borrowed_range<R>;

#endif

}


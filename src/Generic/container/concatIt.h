#pragma once

namespace Engine {

template <typename... T>
struct ConcatItContainer {
    using iterator_traits = derive_iterator<typename first<T...>::type>;

    template <typename... It>
    struct ConcatIterator {
        using iterator_category = typename first<It...>::type::iterator_category;
        using value_type = typename first<It...>::type::value_type;
        using difference_type = ptrdiff_t;
        using pointer = void;
        using reference = typename first<It...>::type::reference;

        ConcatIterator(std::tuple<std::pair<It, It>...> it)
            : mIt(std::move(it))
        {
            update();
        }

        ConcatIterator(const ConcatIterator<It...> &other) = default;
        ConcatIterator(ConcatIterator<It...> &&other) = default;
        ConcatIterator &operator=(const ConcatIterator<It...> &other)
        {
            mIt = other.mIt;
            return *this;
        }
        ConcatIterator &operator=(ConcatIterator<It...> &&other)
        {
            mIt = std::move(other.mIt);
            return *this;
        }

        void operator++()
        {
            assert(mIndex < sizeof...(It));
            TupleUnpacker::select(
                mIt, [](auto &it) -> decltype(auto) {
                    ++it.first;
                },
                mIndex);
            update();
        }

        decltype(auto) operator*() const
        {
            return TupleUnpacker::select(
                mIt, [](auto &it) -> decltype(auto) {
                    return *it.first;
                },
                mIndex);
        }

        bool operator!=(const ConcatIterator<It...> &other) const
        {
            return mIt != other.mIt;
        }

        bool operator==(const ConcatIterator<It...> &other) const
        {
            return mIt == other.mIt;
        }

    private:
        void update()
        {
            while (mIndex < sizeof...(It) && TupleUnpacker::select(
                       mIt, [](auto &it) -> decltype(auto) {
                           return it.first == it.second;
                       },
                       mIndex))
                ++mIndex;
        }

    private:
        std::tuple<std::pair<It, It>...> mIt;
        size_t mIndex = 0;
    };

    using iterator = ConcatIterator<typename derive_iterator<T>::iterator...>;
    using const_iterator = ConcatIterator<typename derive_iterator<T>::const_iterator...>;
    using reverse_iterator = ConcatIterator<typename derive_iterator<T>::reverse_iterator...>;
    using const_reverse_iterator = ConcatIterator<typename derive_iterator<T>::const_reverse_iterator...>;

private:
    ConcatItContainer() = delete;
    ConcatItContainer(const ConcatItContainer<T...> &) = delete;

public:
    ConcatItContainer(ConcatItContainer<T...> &&) = default;

    ConcatItContainer(T &&... containers)
        : mContainers(std::forward<T>(containers)...)
    {
    }

    iterator begin()
    {
        return { TupleUnpacker::forEach(mContainers, [](auto &c) { return std::make_pair(c.begin(), c.end()); }) };
    }

    const_iterator begin() const
    {
        return { TupleUnpacker::forEach(mContainers, [](auto &c) { return std::make_pair(c.begin(), c.end()); }) };
    }

    iterator end()
    {
        return { TupleUnpacker::forEach(mContainers, [](auto &c) { return std::make_pair(c.end(), c.end()); }) };
    }

    const_iterator end() const
    {
        return { TupleUnpacker::forEach(mContainers, [](auto &c) { return std::make_pair(c.end(), c.end()); }) };
    }

    reverse_iterator rbegin()
    {
        return { TupleUnpacker::forEach(mContainers, [](auto &c) { return std::make_pair(c.rbegin(), c.rend()); }) };
    }

    const_reverse_iterator rbegin() const
    {
        return { TupleUnpacker::forEach(mContainers, [](auto &c) { return std::make_pair(c.rbegin(), c.rend()); }) };
    }

    reverse_iterator rend()
    {
        return { TupleUnpacker::forEach(mContainers, [](auto &c) { return std::make_pair(c.rend(), c.rend()); }) };
    }

    const_reverse_iterator rend() const
    {
        return { TupleUnpacker::forEach(mContainers, [](auto &c) { return std::make_pair(c.rend(), c.rend()); }) };
    }

    bool empty() const
    {
        return TupleUnpacker::accumulate(
            mContainers, [](auto &c, bool b) { return c.empty() && b; }, true);
    }

    size_t size() const
    {
        return TupleUnpacker::accumulate(
            mContainers, [](auto &c, size_t s) { return c.size() + s; }, 0);
    }

    bool isReference() const
    {
        constexpr auto check = [](auto &&c) {
            if constexpr (std::is_reference_v<decltype(c)>) {
                return true;
            } else if constexpr (has_function_isReference_v<decltype(c)>) {
                return c.isReference();
            } else {
                return false;
            }
        };
        bool first = check(std::get<0>(mContainers));
        TupleUnpacker::forEach(std::move(mContainers), [=](auto &&c) { assert(check(c) == first); });
        return first;
    }

private:
    std::tuple<T...> mContainers;
};

template <typename... T>
ConcatItContainer<T...> concatIt(T &&... t)
{
    return { std::forward<T>(t)... };
}

}
#pragma once

namespace Engine {

template <typename T, typename Converter>
struct TransformItContainer : T {
    using iterator_traits = derive_iterator<T>;

    template <typename It>
    struct TransformIterator
    {
        using iterator_category = typename It::iterator_category;
        using value_type = typename std::remove_reference<decltype(std::declval<Converter>()(std::declval<typename It::reference>()))>::type;
        using difference_type = ptrdiff_t;
        using pointer = void;
        using reference = value_type;

        TransformIterator(It && it)
            : mIt(std::forward<It>(it))
        {
        }

        void operator++()
        {
            ++mIt;
        }

        decltype(auto) operator*() const
        {
            return Converter {}(*mIt);
        }

        bool operator!=(const TransformIterator<It> &other) const
        {
            return mIt != other.mIt;
        }

        bool operator==(const TransformIterator<It> &other) const
        {
            return mIt == other.mIt;
        }

        std::ptrdiff_t operator-(const TransformIterator<It> &other) const
        {
            return mIt - other.mIt;
        }

    private:
        It mIt;
    };

    using iterator = TransformIterator<typename iterator_traits::iterator>;
    using const_iterator = TransformIterator<typename iterator_traits::const_iterator>;
    using reverse_iterator = TransformIterator<typename iterator_traits::reverse_iterator>;
    using const_reverse_iterator = TransformIterator<typename iterator_traits::const_reverse_iterator>;

private:
    TransformItContainer() = delete;
    TransformItContainer(const TransformItContainer<T, Converter> &) = delete;
    TransformItContainer(TransformItContainer<T, Converter> &&) = delete;

public:
    iterator begin()
    {
        return T::begin();
    }

    const_iterator begin() const
    {
        return T::begin();
    }

    iterator end()
    {
        return T::end();
    }

    const_iterator end() const
    {
        return T::end();
    }

    reverse_iterator rbegin()
    {
        return T::rbegin();
    }

    const_reverse_iterator rbegin() const
    {
        return T::rbegin();
    }

    reverse_iterator rend()
    {
        return T::rend();
    }

    const_reverse_iterator rend() const
    {
        return T::rend();
    }
};

template <typename Converter, typename T>
const TransformItContainer<T, Converter> &transformIt(const T &t)
{
    return static_cast<const TransformItContainer<T, Converter> &>(t);
}

template <typename Converter, typename T>
TransformItContainer<T, Converter> &transformIt(T &t)
{
    return static_cast<TransformItContainer<T, Converter> &>(t);
}

template <typename Converter, typename T>
const TransformItContainer<T, Converter> &transformIt(const T &&t)
{
    static_assert(dependent_bool<T, false>::value, "rvalues are not allowed for transformIt"); //Consider returning temporary
}

template <typename Converter, typename T>
TransformItContainer<T, Converter> &transformIt(T &&t)
{
    static_assert(dependent_bool<T, false>::value, "rvalues are not allowed for transformIt"); //Consider returning temporary
}

struct ToPointerConverter {
    template <typename T>
    T *operator()(T &t)
    {
        return &t;
    }
};

template <typename T>
decltype(auto) toPointer(T &&t)
{
    return transformIt<ToPointerConverter>(std::forward<T>(t));
}

struct UniquePtrToPtrConverter {
    template <typename T, typename D>
    T *operator()(const std::unique_ptr<T, D> &p)
    {
        return p.get();
    }
};

template <typename T>
decltype(auto) uniquePtrToPtr(T &&t)
{
    return transformIt<UniquePtrToPtrConverter>(std::forward<T>(t));
}

}
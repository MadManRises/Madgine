#pragma once

namespace Engine {

template <typename T, typename Converter>
struct TransformItContainer {
    using iterator_traits = derive_iterator<T>;

    template <typename It>
    struct TransformIterator {
        using iterator_category = typename It::iterator_category;
        using value_type = typename std::remove_reference<decltype(std::declval<Converter>()(std::declval<typename It::reference>()))>::type;
        using difference_type = ptrdiff_t;
        using pointer = void;
        using reference = value_type;

        TransformIterator() = default;

        TransformIterator(It &&it, Converter &conv)
            : mIt(std::forward<It>(it))
            , mConverter(&conv)
        {
        }

        TransformIterator(const TransformIterator<It> &other) = default;
        TransformIterator(TransformIterator<It> &&other) = default;
        TransformIterator& operator=(const TransformIterator<It>& other) {
            assert(!mConverter || mConverter == other.mConverter);
            mIt = other.mIt;
            return *this;
        }
        TransformIterator &operator=(TransformIterator<It> &&other)
        {
            assert(!mConverter || mConverter == other.mConverter);
            mIt = std::move(other.mIt);
            return *this;
        }

        TransformIterator &operator++()
        {
            ++mIt;
            return *this;
        }

        TransformIterator operator++(int)
        {
            TransformIterator copy = *this;
            ++*this;
            return copy;
        }

        decltype(auto) operator*() const
        {
            return (*mConverter)(*mIt);
        }

        bool operator!=(const TransformIterator<It> &other) const
        {
            return mIt != other.mIt;
        }

        bool operator==(const TransformIterator<It> &other) const
        {
            return mIt == other.mIt;
        }

        difference_type operator-(const TransformIterator<It> &other) const
        {
            return mIt - other.mIt;
        }

        TransformIterator<It> operator+(difference_type diff) const
        {
            return { mIt + diff, *mConverter };
        }

    private:
        It mIt;
        Converter *mConverter = nullptr;
    };

    using iterator = TransformIterator<typename iterator_traits::iterator>;
    using const_iterator = TransformIterator<typename iterator_traits::const_iterator>;
    using reverse_iterator = TransformIterator<typename iterator_traits::reverse_iterator>;
    using const_reverse_iterator = TransformIterator<typename iterator_traits::const_reverse_iterator>;

private:
    TransformItContainer() = delete;
    TransformItContainer(const TransformItContainer<T, Converter> &) = delete;


public:
    TransformItContainer(TransformItContainer<T, Converter> &&) = default;

    TransformItContainer(T &&container, Converter conv)
        : mContainer(std::forward<T>(container))
        , mConverter(std::move(conv))
    {
    }

    iterator begin()
    {
        return { mContainer.begin(), mConverter };
    }

    const_iterator begin() const
    {
        return { mContainer.begin(), mConverter };
    }

    iterator end()
    {
        return { mContainer.end(), mConverter };
    }

    const_iterator end() const
    {
        return { mContainer.end(), mConverter };
    }

    reverse_iterator rbegin()
    {
        return { mContainer.rbegin(), mConverter };
    }

    const_reverse_iterator rbegin() const
    {
        return { mContainer.rbegin(), mConverter };
    }

    reverse_iterator rend()
    {
        return { mContainer.rend(), mConverter };
    }

    const_reverse_iterator rend() const
    {
        return { mContainer.rend(), mConverter };
    }

    bool empty() const {
        return mContainer.empty();
    }

    size_t size() const {
        return mContainer.size();
    }

    bool isReference() const {
        if constexpr (std::is_reference_v<T>)
            return true;
        else if constexpr (has_function_isReference_v<T>)
            return mContainer.isReference();
        else
            return false;
    }

private:
    T mContainer;
    mutable Converter mConverter;
};

template <typename Converter, typename T>
TransformItContainer<T, Converter> transformIt(T &&t, Converter conv = {})
{
    return { std::forward<T>(t), conv };
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

template <typename T>
decltype(auto) pairSecond(T &&t)
{
    return std::forward<T>(t).second;
}

constexpr auto projectionPairSecond = LIFT(pairSecond);

template <typename T>
decltype(auto) addressOf(T& t)
{
    return &t;
}

constexpr auto projectionAddressOf = LIFT(addressOf);

template <typename T>
decltype(auto) toRawPtr(const T &t)
{
    return t.get();
}

constexpr auto projectionToRawPtr = LIFT(toRawPtr);


}
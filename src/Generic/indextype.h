#pragma once

namespace Engine {

template <Unsigned T, T invalid = std::numeric_limits<T>::max()>
struct IndexType {

    using value_type = T;
    static constexpr T sInvalid = invalid;

    IndexType(T index = invalid)
        : mIndex(index)
    {
    }

    IndexType(const IndexType &) = default;

    IndexType(IndexType &&other)
        : mIndex(std::exchange(other.mIndex, invalid))
    {
    }

    template <std::convertible_to<T> U>
    IndexType(IndexType<U> other)
        : mIndex(other)
    {
    }

    IndexType &operator=(T t)
    {
        assert(t != invalid);
        mIndex = t;
        return *this;
    }

    IndexType &operator=(const IndexType &) = default;

    IndexType& operator=(IndexType&& other) {
        mIndex = std::exchange(other.mIndex, invalid);
        return *this;
    }

    void operator++()
    {
        assert(mIndex != invalid);
        ++mIndex;
    }
    void operator--()
    {
        assert(mIndex != invalid);
        --mIndex;
    }
    IndexType &operator+=(T s)
    {
        assert(mIndex != invalid);
        mIndex += s;
        return *this;
    }
    IndexType &operator-=(T s)
    {
        assert(mIndex != invalid);
        mIndex -= s;
        return *this;
    }

    bool operator==(const IndexType &other) const
    {
        return mIndex == other.mIndex;
    }

    template <std::constructible_from<T> U>
    bool operator==(const U &other) const
    {
        return mIndex == other;
    }

    template <std::constructible_from<T> U>
    bool operator!=(const U &other) const
    {
        return mIndex != other;
    }

    void reset()
    {
        mIndex = invalid;
    }

    operator T() const
    {
        return mIndex;
    }

    explicit operator bool() const
    {
        return mIndex != invalid;
    }

    friend std::ostream &operator<<(std::ostream &out, const IndexType &val)
    {
        int64_t rep = val ? static_cast<int64_t>(val) : -1;
        return out << rep;
    }

    friend std::istream &operator>>(std::istream &in, IndexType &val)
    {
        int64_t rep;
        if (in >> rep) {
            if (rep == -1)
                val.reset();
            else
                val = rep;
        }
        return in;
    }

private:
    T mIndex;
};

}

namespace std {
template <typename T, typename U, template <typename> class TQual, template <typename> class UQual>
struct basic_common_reference<Engine::IndexType<T>, U, TQual, UQual> {
    using type = std::common_reference_t<T, U>;
};
template <typename T, typename U, template <typename> class TQual, template <typename> class UQual>
struct basic_common_reference<T, Engine::IndexType<U>, TQual, UQual> {
    using type = std::common_reference_t<T, U>;
};
}
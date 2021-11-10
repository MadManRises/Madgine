#pragma once

#include "tupleunpacker.h"

namespace Engine {

template <typename T, T invalid = std::numeric_limits<T>::max()>
struct IndexType {

    using value_type = T;
    static constexpr T sInvalid = invalid;

    IndexType(T index = invalid)
        : mIndex(index)
    {
    }

    template <std::convertible_to<T> U>
    IndexType(IndexType<U> other)
        : mIndex(other)
    {
    }

    IndexType &operator=(T t)
    {
        mIndex = t;
        return *this;
    }

    void operator++() { ++mIndex; }
    void operator--() { --mIndex; }
    IndexType &operator-=(T s)
    {
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

    operator T &()
    {
        return mIndex;
    }

    operator T() const
    {
        return mIndex;
    }

    explicit operator bool() const
    {
        return mIndex != invalid;
    }

    explicit operator bool()
    {
        return mIndex != invalid;
    }

private:
    T mIndex;
};

MAKE_NOT_TUPLEFYABLE(SINGLE_ARG(IndexType<T, invalid>), typename T, T invalid)

}
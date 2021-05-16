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

    IndexType &operator=(T t)
    {
        assert(t != invalid);
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
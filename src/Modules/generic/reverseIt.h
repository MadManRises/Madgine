#pragma once

#include "iterator_traits.h"

namespace Engine {

template <typename T>
struct ReverseItContainer : T {
	using traits = derive_iterator<T>;

private:
    ReverseItContainer() = delete;
    ReverseItContainer(const ReverseItContainer<T> &) = delete;
    ReverseItContainer(ReverseItContainer<T> &&) = delete;

public:
    using iterator = typename traits::reverse_iterator;
    using const_iterator = typename traits::const_reverse_iterator;
	using reverse_iterator = typename traits::iterator;
	using const_reverse_iterator = typename traits::const_iterator;

    iterator begin()
    {
        return T::rbegin();
    }

    const_iterator begin() const
    {
        return T::rbegin();
    }

    iterator end()
    {
        return T::rend();
    }

    const_iterator end() const
    {
        return T::rend();
    }

	reverse_iterator rbegin()
    {
        return T::begin();
    }

    const_reverse_iterator rbegin() const
    {
        return T::begin();
    }

    reverse_iterator rend()
    {
        return T::end();
    }

    const_reverse_iterator rend() const
    {
        return T::end();
    }
};

template <typename T>
const ReverseItContainer<T> &reverseIt(const T &t)
{
    return static_cast<const ReverseItContainer<T> &>(t);
}

template <typename T>
ReverseItContainer<T> &reverseIt(T &t)
{
    return static_cast<ReverseItContainer<T> &>(t);
}

}
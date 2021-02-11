#pragma once

#include "Generic/keyvalue.h"

namespace Engine {

template <typename It>
struct KeyValueIterator {
    using iterator_category = typename It::iterator_category;
    using value_type = typename It::value_type;
    using difference_type = typename It::difference_type;
    using pointer = typename It::pointer;
    using reference = typename It::reference;

    KeyValueIterator(It &&it)
        : mIt(std::move(it))
    {
    }

    bool operator==(const KeyValueIterator<It> &other) const
    {
        return mIt == other.mIt;
    }

    void operator++()
    {
        ++mIt;
    }

    auto operator*() const
    {
        decltype(auto) k = kvKey(*mIt);
        decltype(auto) v = kvValue(*mIt);
        return std::pair<decltype(k), decltype(v)>(k, v);
    }

    difference_type operator-(const KeyValueIterator<It> &other)
    {
        return mIt - other.mIt;
    }

private:
    It mIt;
};

}
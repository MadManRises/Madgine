#pragma once

#include "indexholder.h"

namespace Engine {

struct IndexRef {
    IndexHolder *mHolder;

    operator size_t() const
    {
        return mHolder->index();
    }

    bool isValid() const
    {
        return mHolder->isValid();
    }
};

template <typename T>
IndexRef indexRef()
{
    return { &_reg<T, typename T::Collector>() };
}

template <typename T>
IndexRef virtualIndexRef()
{
    return { T::_preg() };
}


}
#pragma once

#include "uniquecomponent.h"

namespace Engine {

#if ENABLE_PLUGINS

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
    return { &_reg<T>() };
}

template <typename T>
IndexRef virtualIndexRef()
{
    return { T::_preg() };
}

#else

struct IndexRef {
    size_t mIndex;

    operator size_t() const
    {
        return mIndex;
    }

    bool isValid() const
    {
        return true;
    }
};

template <typename T>
IndexRef indexRef()
{
    return { component_index<T>() };
}

template <typename T>
IndexRef virtualIndexRef()
{
    return { component_index<T>() };
}

#endif

}
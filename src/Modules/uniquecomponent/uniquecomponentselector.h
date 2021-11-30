#pragma once

#include "uniquecomponentregistry.h"

namespace Engine {

template <typename Registry, typename __Base, typename... _Ty>
struct UniqueComponentSelector
{
    typedef typename Registry::Base Base;

    UniqueComponentSelector(_Ty... arg, size_t index = 0)
        : mValue(Registry::sComponents().at(index)(arg...))
    {
    }

    UniqueComponentSelector(const UniqueComponentSelector &)
        = delete;
    void operator=(const UniqueComponentSelector &) = delete;

    Base *operator->() const
    {
        return mValue.get();
    }

    operator Base *() const
    {
        return mValue.get();
    }

    Base *get() const
    {
        return mValue.get();
    }

private:
    std::unique_ptr<Base> mValue;
};

}